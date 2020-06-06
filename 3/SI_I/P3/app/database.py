# -*- coding: utf-8 -*-

import os
import sys, traceback
from sqlalchemy import create_engine
from sqlalchemy import Table, Column, Integer, String, MetaData, ForeignKey, text
from sqlalchemy.sql import select

# configurar el motor de sqlalchemy
db_engine = create_engine("postgresql://alumnodb:alumnodb@localhost/si1", echo=False)
db_meta = MetaData(bind=db_engine)


# db_movies_1949 = select([db_table_movies]).where(text("year = '1949'"))

def findCustomer(usuario):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # cargar la tabla sobre la que hacer cosas
        db_table_customers = Table('customers', db_meta, autoload=True, autoload_with=db_engine)

        # Seleccionar el customer con ese username a ver si existe
        existing_customers = select([db_table_customers]).where(text("username = '%s'"%usuario))
        db_result = db_conn.execute(existing_customers)

        if len(list(db_result)) > 0:
            return 1

        db_conn.close()
        return 0

    except:
        return dbException(db_conn)


def registerCustomer(usuario, clave, correo, tarjeta, saldo):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # cargar la tabla sobre la que hacer cosas
        db_table_customers = Table('customers', db_meta, autoload=True, autoload_with=db_engine)

        # Insertar un nuevo customer
        db_result = db_conn.execute(db_table_customers.insert(), username = usuario, password = clave, email = correo, creditcard = tarjeta, income = saldo)

        db_conn.close()
        return db_result

    except:
        return dbException(db_conn)


def getCustomer(campo, valor):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # cargar la tabla sobre la que hacer cosas
        db_table_customers = Table('customers', db_meta, autoload=True, autoload_with=db_engine)
        # print(usuario)

        # Seleccionar el customer con ese campo a ver si existe
        existing_customers = select([db_table_customers]).where(text("%s = '%s'"%(campo,valor)))
        db_result = db_conn.execute(existing_customers)

        row = db_result.fetchone()
        # print("name:", row['username'], "; fullname:", row['password'])

        if row:
            return row

        db_conn.close()
        return -1

    except:
        return dbException(db_conn)


def getOrder(campo , valor):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # cargar la tabla sobre la que hacer cosas
        db_table_order= Table('orders', db_meta, autoload=True, autoload_with=db_engine)
        # print(usuario)

        # Seleccionar el customer con ese campo a ver si existe
        existing_order= select([db_table_order]).where(text(("%s = %s "%(campo,valor)) if valor else ("%s is not NULL "%campo)))
        db_result = db_conn.execute(existing_order)

        row = db_result.fetchone()
        # print("name:", row['username'], "; fullname:", row['password'])

        if row:
            return row

        db_conn.close()
        return -1

    except:
        return dbException(db_conn)

def getOrderWhere(where):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # cargar la tabla sobre la que hacer cosas
        db_table_order= Table('orders', db_meta, autoload=True, autoload_with=db_engine)
        # print(usuario)

        # Seleccionar el customer con ese campo a ver si existe
        existing_order= select([db_table_order]).where(text( where))
        db_result = db_conn.execute(existing_order)

        row = db_result.fetchone()
        # print("name:", row['username'], "; fullname:", row['password'])

        if row:
            return row

        db_conn.close()
        return -1

    except:
        return dbException(db_conn)


def newOrder(usuario, date):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # cargar la tabla sobre la que hacer cosas
        db_table_orders= Table('orders', db_meta, autoload=True, autoload_with=db_engine)

        # comprobar si hay algun carrito con estado null
        newOrder = getOrderWhere("customerid ='" + str(usuario) + "' and status is null")
        if newOrder != -1:
             return newOrder
        # Insertar un nuevo customer
        createOrder(usuario, date)
        db_conn.close()
        return  getOrderWhere("customerid ='" + str(usuario) + "' and status is null")

    except:
        return dbException(db_conn)


def createOrder(usuario, date):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # cargar la tabla sobre la que hacer cosas
        db_table_orders= Table('orders', db_meta, autoload=True, autoload_with=db_engine)

        
        # Insertar un nuevo customer
        db_result = db_conn.execute(db_table_orders.insert(), customerid = usuario, orderdate = date, tax =21,status = None)
        db_conn.close()
        return db_result

    except:
        return dbException(db_conn)



def getTop():
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # obtener la lista del top ventas
        db_result = db_conn.execute("select * from gettopventas(CAST (EXTRACT(YEAR FROM CURRENT_DATE) AS INTEGER) - 2)")
        top = []
        for row in db_result:
            pelicula= {}
            id = db_conn.execute("select * from imdb_movies where movietitle = '%s'"%row[1]).fetchone()['movieid']
            
            pelicula['id'] = id
            pelicula['titulo'] = row[1]
            pelicula['poster'] = "images/noImage.png"
            top.append(pelicula)
            

        db_conn.close()
       
        return top

    except:
        return dbException(db_conn)

def getGenres():
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # obtener la lista del top ventas
        db_result = db_conn.execute("select genre from  genres")
        
        db_conn.close()
       
        return list(db_result)

    except:
        return dbException(db_conn)


def getSearch(search):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # obtener la lista del top ventas
        query = "select movieid,movietitle from  imdb_movies\
                 where Lower(movietitle) like Lower('%%" + str(search) + "%%')"
        db_result = db_conn.execute(query)
        result = []
        for row in db_result:
            pelicula= {}
            
            pelicula['id'] = row[0]
            pelicula['titulo'] = row[1]
            pelicula['poster'] = "images/noImage.png"
            result.append(pelicula)

        db_conn.close()
       
        return result

    except:
        return dbException(db_conn)

def getSearchByGenre(search,genre):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # obtener la lista del top ventas
        query = " select movieid,movietitle, genre from  imdb_movies\
                  natural join imdb_moviegenres natural join genres\
                  where Lower(movietitle) like Lower('%%" + str(search) + "%%')\
                  and genre = '" + str(genre) + "'"
        db_result = db_conn.execute(query)
        result = []
        for row in db_result:
            pelicula= {}
            
            pelicula['id'] = row[0]
            pelicula['titulo'] = row[1]
            pelicula['poster'] = "images/noImage.png"
            result.append(pelicula)

        db_conn.close()
       
        return result

    except:
        return dbException(db_conn)

"""
Esta funcion devuelve un diccionario con los datos de la pelicula solicitada.
El reparto se limita a 10 entradas ordenadas por orden de aparicon en los creditos
"""

def getFilmData(id,product = None):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # obtener la lista del top ventas
        query = "select movieid,movietitle, year from  imdb_movies where movieid = %d"%id
        row = db_conn.execute(query).fetchone()
        query = "select price,prod_id from products where movieid= %d "%id 
        if product:
            query += " and prod_id = %d"%product
        else:
            query += "order by price desc"
        precio = db_conn.execute(query).fetchone()
        director = db_conn.execute("select directorname from imdb_directormovies natural join imdb_directors where movieid = %d"%id ).fetchone()[0]
        categorias = db_conn.execute("select genre from imdb_moviegenres natural join genres where movieid = %d"%id )
        categoria = ''
        for cat in categorias:
            categoria+= cat[0] + ", "
        
        paises = db_conn.execute("select country from imdb_moviecountries natural join countries where movieid = %d"%id )
        pais = ''
        for p in paises:
            pais+= p[0] + ", "

        actors = db_conn.execute("select actorname, character from imdb_actors\
                                  natural join imdb_actormovies where movieid = %d\
                                  order by creditsposition limit 8"%id  )
        actores = []
        for act in actors:
            actores.append({ "nombre": act[0], "personaje": act[1]})

        pelicula= {}
        pelicula['id'] = id
        pelicula['año']= row[2]
        pelicula['poster'] = "images/noImage.png"
        pelicula['precio']= float("{0:.2f}".format(precio[0]))
        pelicula['titulo']= row[1]
        pelicula['director'] = director
        pelicula['categoria'] = categoria[:-2]
        pelicula['pais'] = pais[:-2]
        pelicula['producto'] = precio[1]
        pelicula['actores'] = actores
        
        db_conn.close()
       
        return pelicula

    except:
        return dbException(db_conn)


"""
Esta funcion devuelve crea un nuevo orderdetail. Si el order ditail 
que se solicita ya se encontrase en la tabla, se hara un update con la nueva cantidad
"""
def createOrderDetai(orderId, productId, cost, units):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # cargar la tabla sobre la que hacer cosas
        db_table_orderdetail= Table('orderdetail', db_meta, autoload=True, autoload_with=db_engine)

        # comprombamos si el oreden detail solicitado ya se encuentra en la tabla
        existing_order= select([db_table_orderdetail]).where(text( "orderid = " + str(orderId) + " and prod_id = " + str(productId) ))
        db_result = db_conn.execute(existing_order)
        row = db_result.fetchone()
        
        if row:
            cantidad = row['quantity'] + units
            query = " update orderdetail set quantity = " + str(cantidad) + " where orderid = " + str(orderId) + " and prod_id = " + str(productId)
            db_result = db_conn.execute(query)
            return db_result
        

        # Insertar un nuevo orderdetail
        db_result = db_conn.execute(db_table_orderdetail.insert(), orderid = orderId, prod_id = productId,\
                                    price = cost, quantity = units)
        db_conn.close()
        return db_result

    except:
        return dbException(db_conn)


"""
Esta funcion borra una entrada de la tabala orederdetail
"""
def deleteOrderDetai(orderId, productId):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # comprombamos si el oreden detail solicitado ya se encuentra en la tabla
        query = " delete from orderdetail where orderid = " + str(orderId) + " and prod_id = " + str(productId)
        db_result = db_conn.execute(query)

        db_conn.close()
        return db_result

    except:
        return dbException(db_conn)

"""
Esta funcion genera un diccionario con las productos que tiene el pedido solicitado.
Se incluye la informacion de la pelicula asociada a dicho producto
"""
def getCarrito(orderId):
    try:
        # conexion a la base de datos
        db_conn = None
        db_conn = db_engine.connect()

        # obtener la lista del top ventas
        query = " select * from orderdetail where orderid =" + str(orderId)
        db_result = db_conn.execute(query)
        result = {}
        for row in db_result:
            product= {}
            pid = row['prod_id']
            product['id'] = pid
            product['cantidad'] = row['quantity']
            product['precio'] = float("{0:.2f}".format(row['price']))
            query = " select movieid from products where prod_id =" + str(pid)
            filmid = db_conn.execute(query).fetchone()[0]
            product['pelicula'] = getFilmData(filmid,pid)
            product['pelicula']['precio'] =  product['precio'] 
            result[pid]= product

        db_conn.close()
       
        return result

    except:
        return dbException(db_conn)


def dbException(db):
    if db is not None:
            db.close()
    print("Exception in DB access:")
    print("-"*60)
    traceback.print_exc(file=sys.stderr)
    print("-"*60)

    return 'Something is broken'



