#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from app import app
from flask import render_template, request, url_for, redirect, session, flash, make_response
from datetime import date
import json
import os
import sys
import hashlib
import random


@app.route('/')
@app.route('/index', methods=['GET', 'POST'])
def index():
    catalogue_data = open(os.path.join(app.root_path,'catalogue/catalogue.json'), encoding="utf-8").read()
    catalogue = json.loads(catalogue_data)
    movies_list = []
    opcion = None
    busqueda = None
    if not 'carrito' in session:
        session['carrito']=[]
    if request.method == "POST":
        opcion = request.form['options']
        busqueda = request.form['search']

        if opcion == 'Todo':
            for m in catalogue:
                if m['titulo'].lower().rfind(busqueda.lower().strip()) != -1:
                    movies_list.append (m)
        else:
            for m in catalogue:
                if m['categoria'].lower().rfind(opcion.lower().strip()) != -1 and m['titulo'].lower().rfind(busqueda.lower().strip()) != -1:
                    movies_list.append (m)

        catalogue = movies_list

    return render_template('index.html', title = "Home", title_main = "Catalogo", movies=catalogue, search=busqueda, dropValue=opcion )


@app.route('/detail/<int:movie_id>/', methods=['POST', 'GET'])
def detail(movie_id = 0):
    msg = None
    catalogue_data = open(os.path.join(app.root_path,'catalogue/catalogue.json'), encoding="utf-8").read()
    catalogue = json.loads(catalogue_data)

    if request.method == 'POST':
        itemId = int(request.form['idItem'])
        pos = itemInDictIndex(session['carrito'],'id', itemId)
        if pos != -1:
            session['carrito'][pos]['cantidad'] += 1
            msg = "¡%s ya esta en el carrito, se han aumentado las unidades!"%catalogue[itemId]['titulo']
        else:
            d = {'id': itemId,
                'cantidad': 1}
            session['carrito'].append(d)
            msg = "¡%s añadido al carrito!"%catalogue[itemId]['titulo']

        session.modified = True


    for m in catalogue:
        if movie_id == m['id']:
            mov = m
    return render_template('detail.html', title = "Detalle", title_main = mov['titulo'], movie = mov, message=msg)

@app.route('/history')
def history():
    catalogue_data = open(os.path.join(app.root_path,'catalogue/catalogue.json'), encoding="utf-8").read()
    catalogue = json.loads(catalogue_data)
    userPath = os.path.join(app.root_path,'../usuarios/%s/'%session['usuario'])
    history_data = open(os.path.join(userPath, 'historial.json'), encoding="utf-8").read()
    
    historydat = {}

    if history_data:
        historydat = json.loads(history_data)
        
    return render_template('history.html', title = "Historial", catalogo = catalogue,
                            history = historydat)

@app.route('/about')
def about():
    return render_template('about.html', title = "Sobre Nosotros")

@app.route('/carrito', methods=['POST', 'GET'])
def carrito():
    catalogue_data = open(os.path.join(app.root_path,'catalogue/catalogue.json'), encoding="utf-8").read()
    catalogue = json.loads(catalogue_data)
    msg = None

    if request.method == 'POST':
        if 'cartAction' in request.form:
            if request.form['cartAction'] == 'comprar' and len(session['carrito'])>0:
                if 'usuario'in session :
                    userPath = os.path.join(app.root_path,'../usuarios/%s/'%session['usuario'])
                    userDataFile = open(os.path.join(userPath, 'datos.dat'),"r")
                    dataContent = userDataFile.readlines()
                    userDataFile.close()
                    saldo = float(dataContent[4])

                    suma = getCartTotal(catalogue)
                    if suma > saldo:
                        msg = 'No dispone de saldo suficiente'
                    else:
                        print(dataContent)

                        userDataFile = open(os.path.join(userPath, 'datos.dat'),"w")
                        for line in dataContent[: -1]:
                            userDataFile.write(line)
                        saldo -= suma
                        userDataFile.write("%.2f"%saldo + "\n")
                        userDataFile.close()

                        
                        history_data = open(os.path.join(userPath, 'historial.json'), encoding="utf-8").read()
                        history = json.loads(history_data)
                        print("historial: %s"%str(history))

                        today = date.today()
                        tdate = today.strftime("%d/%m/%Y")
                        jsonData ={ "precio": suma ,"fecha":tdate, "estado": "pendiente", 
                                    "articulos":session['carrito']}
                        if len(history) < 1:
                            history[0] = jsonData
                        else:
                            keyList = list(history.keys())
                            lastId = int(sorted(keyList)[-1]) +1
                            history[str(lastId)] = jsonData

                        with open(os.path.join(userPath, 'historial.json'), "w") as write_file:
                            json.dump(history, write_file, indent=4, sort_keys=True)
                        session['carrito'] = []
                        session.modified=True
                        msg = 'Compra realizada, saldo restante: %.2f€ '%saldo

                else:
                    msg = 'Debe iniciar sesion para poder realizar la compra'

            elif request.form['cartAction'] == 'delete':
                items = request.form['selectedItems']
                for id in items.replace(',', ''):
                    session['carrito'].remove(session['carrito'][itemInDictIndex(session['carrito'],'id', int(id))])
                    session.modified=True

        else:
            itemId = int(request.form['idItem'])
            nItems = int(request.form['numItems'])
            pos = itemInDictIndex(session['carrito'],'id', itemId)

            if pos != -1:
                session['carrito'][pos]['cantidad'] = nItems
                session.modified=True

    suma = getCartTotal(catalogue)    
    return render_template('carrito.html', title = "Carrito", catalogo = catalogue,
                            carrito = session['carrito'], total =suma , message=msg)

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == "POST":
        user = request.form['user']
        path = os.path.join(app.root_path, "../usuarios/%s"%user)
        carritoPath = os.path.join(app.root_path, "../usuarios/%s/carrito.json"%user)
        if os.path.isdir(path) == True and os.path.exists(path) == True:
            password = request.form['pass']
            datContent = open(path + "/datos.dat",).readlines()
            passwordMd5 = hashlib.md5()
            passwordMd5.update(password.encode("utf-8"))
            codePass = str(passwordMd5.hexdigest())

            if datContent[2].find(codePass) != -1:
                print('iniciando sesion')
                print(carritoPath)
                if os.path.exists(carritoPath) == True:
                    carrito_data = open(carritoPath, encoding="utf-8").read()
                    session['carrito'] += json.loads(carrito_data)
                    print('Recuperando carro')
                    print(session['carrito'])
                session['usuario'] = user
                session.modified=True
                return setcookie(user)
            else:
                errMessage = "contraseña incorrecta "
                return render_template('login.html', title = "Iniciar sesión", error = errMessage)
        else:
            nonExistingUserError ="el usuario %s no existe"%user
            return render_template('login.html', title = "Iniciar sesión", error = nonExistingUserError)

    return render_template('login.html', title = "Login", title_main="Iniciar sesión")

@app.route('/signup', methods=['GET', 'POST'])
def signup():
    existingUserError = ""

    if request.method == "POST":
        user = request.form['user']
        path = os.path.join(app.root_path, "../usuarios/%s"%user)

        if os.path.isdir(path) == True and os.path.exists(path) == True:
            existingUserError ="el usuario %s ya existe"%user
            return render_template('signup.html', title = "Registro", error = existingUserError)

        else:
            email = request.form['email']
            password = request.form['password']
            card = request.form['card']
            saldo = random.randint(0, 100)

            passwordMd5 = hashlib.md5()
            passwordMd5.update(password.encode("utf-8"))

            os.makedirs(path)
            jsonfile = open(path+"/historial.json", 'w')
            jsonfile.write("{\n}\n")
            jsonfile.close()

            with open(path + "/datos.dat","w") as file:
                file.write(user+'\n')
                file.write(email+'\n')
                file.write(passwordMd5.hexdigest()+'\n')
                file.write(card+'\n')
                file.write(str(saldo)+'\n')

            session['usuario'] = request.form['user']
            session.modified=True

            return setcookie(user)

    return render_template('signup.html', title = "Registro")



@app.route('/logout', methods=['GET', 'POST'])
def logout():
    userPath = os.path.join(app.root_path,'../usuarios/%s/'%session['usuario'])
    with open(os.path.join(userPath, 'carrito.json'), "w") as write_file:
        json.dump(session['carrito'], write_file, indent=4, sort_keys=True)
    session['carrito'] = []
    session.pop('usuario', None)
    session.modified=True
    return redirect(url_for('index'))

def itemInDictIndex( lista,searchedValue, searchedItem ):
    for index,element in enumerate(lista):
        if element[searchedValue] == searchedItem:
            return index
    return -1

def getCartTotal(catalogue):
    suma = 0
    for item in session['carrito']:
        suma += catalogue[item['id']]['precio']*item['cantidad']
        print(suma)
    return float("{0:.2f}".format(suma))

@app.route('/rand', methods=['GET', 'POST'])
def generateRandomInt():
    rand = str(random.randint(0, 10))
    print(rand)
    return rand

def setcookie(user):
    resp = make_response(redirect(url_for('index')))
    resp.set_cookie('userID', user)
    return resp


@app.route('/getcookie')
def getcookie():
   name = request.cookies.get('userID')
   print(name)
   return name
