#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from app import app
from app import database
from flask import render_template, request, url_for
import os
import sys
import time

@app.route('/borraCliente', methods=['POST','GET'])
def borraCliente():
    if "customerid" in request.args:
        customerid = request.args.get("customerid")
        bCommit = "bCommit" in request.args
        bFallo  = "bFallo"  in request.args
        duerme  = request.args.get("duerme")
        dbr = database.delCustomer(customerid, bFallo, int(duerme), bCommit)
        return render_template('borraCliente.html', dbr=dbr)
    else:
        return render_template('borraCliente.html')

@app.route('/xSearchInjection', methods=['GET'])
def xSearchInjection():
    if 'i_anio' in request.args:
        anio  = request.args['i_anio']
        dbr = database.getMovies(anio)
        return render_template('xSearchInjection.html', dbr=dbr)
    else:
        return render_template('xSearchInjection.html')

@app.route('/xLoginInjection', methods=['GET','POST'])
def xLoginInjection():
    if 'login' in request.form:
        login  = request.form['login']
        pswd   = request.form['pswd']
        dbr = database.getCustomer(login, pswd)
        return render_template('xLoginInjection.html', dbr=dbr)
    else:
        return render_template('xLoginInjection.html')

@app.route('/listaClientesMes', methods=['GET', 'POST'])
def listaClientesMes():
    if 'fecha' in request.form:
        fecha  = request.form['fecha']
        mes    = request.form['mes']
        anio   = request.form['anio']
        umbral = request.form['minimo']
        intervalo = request.form['intervalo']
        use_prepare = 'prepare'  in request.form
        break0 = 'break0' in request.form
        niter  = request.form['iter']
        # connect fuera para comparar tiempos con más precisión
        db_conn = database.dbConnect()
        t0=round(time.time() * 1000)
        dbr = database.getListaCliMes(db_conn, mes, anio, int(umbral), int(intervalo), use_prepare, break0, int(niter))
        t1=round(time.time() * 1000)
        database.dbCloseConnect(db_conn)

        return render_template('listaClientesMes.html',
            fecha = fecha,
            mes   = mes,
            anio  = anio,
            umbral = int(umbral),
            intervalo = int(intervalo),
            use_prepare = use_prepare,
            break0 = break0,
            tiempo = str(int(t1-t0)),
            dbr=dbr
           )
    else:
        return render_template('listaClientesMes.html')
