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
from app import database


@app.route('/')
@app.route('/index', methods=['GET', 'POST'])
def index():
    catalogue = database.getTop()
    categorys = database.getGenres()
    movies_list = []
    opcion = None
    busqueda = None
    if not 'carrito' in session:
        session['carrito']={}
    if request.method == "POST":
        opcion = request.form['options']
        busqueda = request.form['search']

        if opcion == 'Todo':
            for m in catalogue:
                movies_list = database.getSearch(busqueda)
        else:
            for m in catalogue:
                movies_list = database.getSearchByGenre(busqueda, opcion)

        catalogue = movies_list
    return render_template('index.html', title = "Home", title_main = "Lo más vendido", movies=catalogue, search=busqueda, dropValue=opcion, genres = categorys  )


@app.route('/detail/<int:movie_id>/', methods=['POST', 'GET'])
def detail(movie_id = 0):
    msg = None
    mov = database.getFilmData(movie_id)
    
    if request.method == 'POST':
        if 'usuario'  in session:
            order = session['order'] 
            print(order)

        else:
            itemId = int(request.form['idItem'])
            if itemId in session['carrito']:
                session['carrito'][itemId]['cantidad'] += 1
                msg = "¡%s ya esta en el carrito, se han aumentado las unidades!"%mov['titulo']

            else:
                d = {'id': itemId,
                    'cantidad': 1}
                session['carrito'][itemId] = d
                
                msg = "¡%s añadido al carrito!"%mov['titulo']
        session.modified = True


    mov = database.getFilmData(movie_id)
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
    msg = None
    catalogue = None
    suma = None

    if 'usuario' in session:
        orderId = session['order']['orderid']
        carro = database.getCarrito(orderId)
        catalogue = getCartMovieDict(carro)
    else:
        carro = session['carrito']
        catalogue = getCartMovieDict(carro)

    if request.method == 'POST':
        if 'cartAction' in request.form:
            print ("car action")
            if request.form['cartAction'] == 'comprar' and len(session['carrito'])>0:
                if 'usuario' in session :
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
                        session['carrito'] = {}
                        session.modified=True
                        msg = 'Compra realizada, saldo restante: %.2f€ '%saldo

                else:
                    msg = 'Debe iniciar sesion para poder realizar la compra'

            elif request.form['cartAction'] == 'delete':
                if 'usuario' in session:
                    print("borrando")
                    items = request.form['selectedItems']
                    for id in items.split(","):
                        database.deleteOrderDetai(orderId, int(id))

                else:
                    items = request.form['selectedItems']
                    for id in items.split(","):
                        session['carrito'].pop(int(id))
                        session.modified=True

        else:
            itemId = int(request.form['idItem'])
            nItems = int(request.form['numItems'])

            if 'usuario' in session:
                print()

            else:
                if itemId in session['carrito']:
                    session['carrito'][itemId]['cantidad'] = nItems
                    session.modified=True
    
    if 'usuario' in session:
        orderId = session['order']['orderid']
        suma = database.getOrder('orderid',orderId)['netamount']
        carro = database.getCarrito(orderId)
        catalogue = getCartDict(carro)
    else:
        suma = getCartTotal()
        carro = session['carrito']
        catalogue = getCartMovieDict(carro)
        
    return render_template('carrito.html', title = "Carrito", catalogo = catalogue,
                            carrito = carro, total =suma , message=msg)

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == "POST":
        user = request.form['user']
        userData = database.getCustomer("email", user)
        print(userData)
        if userData != -1:
            print('usuario existe')
            password = request.form['pass']

            if userData['password'] == password:
                print('iniciando sesion')
                session['usuario'] = user
                session.modified=True
                tdate = date.today().strftime("%d/%m/%Y")
                print('usuario almacenado')
                print(userData)
                order= database.newOrder(userData['customerid'],tdate)
                session['order'] = order
                print(order )
                print('sesionnnnnnnnnnnnnnnn')
                print(session['carrito'])
                for k in session['carrito']:
                    c = session['carrito'][k]['cantidad']
                    product = database.getFilmData(session['carrito'][k]['id'])
                    precio =  product['precio'] * c      
                    database.createOrderDetai(order['orderid'], product['producto'], precio, c)

                session['carrito'] = {}
                return setcookie(user)
            else:
                errMessage = "contraseña incorrecta "
                return render_template('login.html', title = "Iniciar sesión", error = errMessage)
        else:
            print('usuario no existe')
            nonExistingUserError ="el usuario %s no existe"%user
            return render_template('login.html', title = "Iniciar sesión", error = nonExistingUserError)

    return render_template('login.html', title = "Login", title_main="Iniciar sesión")

@app.route('/signup', methods=['GET', 'POST'])
def signup():
    existingUserError = ""

    if request.method == "POST":
        email = request.form['email']

        if database.getCustomer("email", email) != -1:
            existingUserError ="el usuario con email %s ya existe"%email
            return render_template('signup.html', title = "Registro", error = existingUserError)

        else:
            user = request.form['user']
            password = request.form['password']
            card = request.form['card']
            saldo = random.randint(0, 100)

            database.registerCustomer(user, password, email, card, saldo)
            userData = database.getCustomer("email", email)
            print('userdataaaa')
            print(userData)
            tdate = date.today().strftime("%d/%m/%Y")
            print('usuario almacenado')
            print(userData)
            order = database.newOrder(userData['customerid'],tdate)
            session['order'] = order
            print(session['carrito'])
            for k in session['carrito']:
                c = session['carrito'][k]['id']
                p =  session['carrito'][k]['id'] * c      
                database.createOrderDetai(order['orderid'], session['carrito'][k]['id'], p, database, c)

            session['usuario'] = request.form['email']
            session['carrito'] = {}
            session.modified=True

            return setcookie(user)

    return render_template('signup.html', title = "Registro")



@app.route('/logout', methods=['GET', 'POST'])
def logout():
    """
    userPath = os.path.join(app.root_path,'../usuarios/%s/'%session['usuario'])
    with open(os.path.join(userPath, 'carrito.json'), "w") as write_file:
        json.dump(session['carrito'], write_file, indent=4, sort_keys=True)
    """
    session['carrito'] = {}
    session.pop('usuario', None)
    session.pop('order', None)
    session.modified=True
    return redirect(url_for('index'))

def itemInDictIndex( lista,searchedValue, searchedItem ):
    for index,element in enumerate(lista):
        if element[searchedValue] == searchedItem:
            return index
    return -1

def getCartTotal():
    suma = 0
    for item in session['carrito']:
        print(item)
        suma += database.getFilmData(item)['precio']*session['carrito'][item]['cantidad']
        print(suma)
    return float("{0:.2f}".format(suma))

def getCartMovieDict(carro, prod = None):
    dicts = {}
    for item in carro:
        dicts[item] = database.getFilmData(item)
        
    return dicts

def getCartDict(carro):
    dicts = {}
    print("haciendo diccionario de usuario")
    for item in carro:
        dicts[carro[item]['id']] = carro[item]['pelicula']
        
    return dicts

@app.route('/rand', methods=['GET', 'POST'])
def generateRandomInt():
    rand = str(random.randint(0, 10))
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
