# -*- coding: utf-8 -*-

import os
import sys, traceback, time

from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker

# configurar el motor de sqlalchemy
db_engine = create_engine("postgresql://alumnodb:alumnodb@localhost/si1", echo=False, execution_options={"autocommit":False})
Session = sessionmaker(bind=db_engine)


def dbConnect():
    return db_engine.connect()

def dbCloseConnect(db_conn):
    db_conn.close()

def getListaCliMes(db_conn, mes, anio, iumbral, iintervalo, use_prepare, break0, niter):
    consultaPreparada=""

    consulta = "select count (distinct orders.customerid) as cc \
                from customers inner join orders \
                on orders.customerid = customers.customerid \
                where extract(year from orderdate)=" + str(anio) + " and \
                	  extract(month from orderdate)=" + str(mes) + " and \
                	  totalamount>"

    if use_prepare is True:
        checkStmt = "SELECT * FROM pg_prepared_statements;"

        res = db_conn.execute(checkStmt).fetchone()

        if res is None:
            print("----------------------Vamos a crear el stmt")
            db_conn.execute("deallocate all;\
                prepare stmt (numeric, numeric, numeric) as\
                select count (distinct orders.customerid) as cc \
                from customers inner join orders \
                on orders.customerid = customers.customerid \
                where extract(year from orderdate)=$1 and \
                	  extract(month from orderdate)=$2 and \
                	  totalamount>$3;")
        else:
            print("++++++++++++++++++++++++++++++++++++++++++++++++")
    # TODO: ejecutar la consulta
    # - mediante PREPARE, EXECUTE, DEALLOCATE si use_prepare es True
    # - mediante db_conn.execute() si es False

    # Array con resultados de la consulta para cada umbral
    dbr=[]

    for ii in range(niter):

        if use_prepare is not True:
            res = db_conn.execute(consulta+str(iumbral)).fetchone()
        else:
            res = db_conn.execute("execute stmt(" + str(anio) + ", " + str(mes) + ", " + str(iumbral) + ");").fetchone()

        # Guardar resultado de la query
        dbr.append({"umbral":iumbral,"contador":res['cc']})

        if break0 is True and res['cc'] == 0:
            break

        # Actualizacion de umbral
        iumbral = iumbral + iintervalo

    return dbr

def getMovies(anio):
    # conexion a la base de datos
    db_conn = db_engine.connect()

    query="select movietitle from imdb_movies where year = '" + anio + "'"
    resultproxy=db_conn.execute(query)

    a = []
    for rowproxy in resultproxy:
        d={}
        # rowproxy.items() returns an array like [(key0, value0), (key1, value1)]
        for tup in rowproxy.items():
            # build up the dictionary
            print("Clave: "+str(tup[0]))
            print("Valor: "+str(tup[1]))
            d[tup[0]] = tup[1]
        a.append(d)

    resultproxy.close()

    db_conn.close()

    return a

def getCustomer(username, password):
    # conexion a la base de datos
    db_conn = db_engine.connect()

    query="select * from customers where username='" + username + "' and password='" + password + "'"
    res=db_conn.execute(query).first()

    db_conn.close()

    if res is None:
        return None
    else:
        return {'firstname': res['firstname'], 'lastname': res['lastname']}

def delCustomer(customerid, bFallo, bSQL, duerme, bCommit):
    db_conn = db_engine.connect()
    session = Session()

    # Array de trazas a mostrar en la página
    dbr=[]
    query=""

    # TODO: Ejecutar consultas de borrado
    # - usar sentencias SQL ('BEGIN', 'COMMIT', ...) si bSQL es True
    # - suspender la ejecución 'duerme' segundos en el punto adecuado para forzar deadlock

    try:
        if bSQL is True:
            if bFallo is True:
                dbr.append("Vamos a ejecutar la transaccion en orden incorrecto (orderdetail, customers, orders) para provocar error de integridad")

                res=db_conn.execute("BEGIN;")
                dbr.append("Begin")
                res=db_conn.execute("DELETE from orderdetail where orderid in (SELECT orderid from orders where customerid=" + str(customerid) + ");").rowcount
                dbr.append("Hemos borrado %d filas de orderdetail"%res)

                res=db_conn.execute("select count(*) from orderdetail where orderid in (select orderid from orders where customerid="+str(customerid)+");").fetchone()
                dbr.append("Ahora en orderdetail no hay filas para este usuario: "+str(res[0]))

                if bCommit is True:
                    dbr.append("Vamos a ejecutar un commit intermedio, antes del error, para guardar los cambios de orderdetail")
                    res=db_conn.execute("COMMIT;")
                    res=db_conn.execute("BEGIN;")

                res=db_conn.execute("DELETE from customers where customerid="+str(customerid) + ";").rowcount
                dbr.append("Hemos borrado %d filas de customers"%res)

                res=db_conn.execute("select count(*) from customers where customerid="+str(customerid)+";").fetchone()
                dbr.append("Ahora en customers no hay filas para este usuario: "+str(res[0]))

                res=db_conn.execute("DELETE from orders where customerid="+ str(customerid) + ";").rowcount
                dbr.append("Hemos borrado %d filas de orders"%res)

                res=db_conn.execute("select count(*) from orders where customerid="+str(customerid)+";").fetchone()
                dbr.append("Ahora en orders no hay filas para este usuario: "+str(res[0]))

            else:
                dbr.append("Vamos a ejecutar la transaccion en orden correcto (orderdetail, orders, customers)")

                res=db_conn.execute("BEGIN;")
                dbr.append("Begin")
                res=db_conn.execute("DELETE from orderdetail where orderid in (SELECT orderid from orders where customerid=" + str(customerid) + ");").rowcount
                dbr.append("Hemos borrado %d filas de orderdetail"%res)

                res=db_conn.execute("select count(*) from orderdetail where orderid in (select orderid from orders where customerid="+str(customerid)+");").fetchone()
                dbr.append("Ahora en orderdetail no hay filas para este usuario: "+str(res[0]))

                res=db_conn.execute("DELETE from orders where customerid="+ str(customerid) + ";").rowcount

                dbr.append("Hemos borrado %d filas de orders"%res)
                res=db_conn.execute("select count(*) from orders where customerid="+str(customerid)+";").fetchone()
                dbr.append("Ahora en orders no hay filas para este usuario: "+str(res[0]))

                res=db_conn.execute("DELETE from customers where customerid="+str(customerid) + ";").rowcount

                dbr.append("Hemos borrado %d filas de customers"%res)
                res=db_conn.execute("select count(*) from customers where customerid="+str(customerid)+";").fetchone()
                dbr.append("Ahora en customers no hay filas para este usuario: "+str(res[0]))

        else:
            dbr.append("Vamos a usar SQLAlchemy")

            if bFallo is True:
                dbr.append("Vamos a ejecutar la transaccion en orden incorrecto (orderdetail, customers, orders) para provocar error de integridad")

                res=session.query("DELETE from orderdetail where orderid in (SELECT orderid from orders where customerid=" + str(customerid) + ");").count()
                dbr.append("Hemos borrado %d filas de orderdetail"%res)

                res=session.query("select count(*) from orderdetail where orderid in (select orderid from orders where customerid="+str(customerid)+");").one()
                dbr.append("Ahora en orderdetail no hay filas para este usuario: "+str(res[0]))

                if bCommit is True:
                    dbr.append("Vamos a ejecutar un commit intermedio, antes del error, para guardar los cambios de orderdetail")
                    res=session.query("COMMIT;")
                    res=session.query("BEGIN;")

                res=session.query("DELETE from customers where customerid="+str(customerid) + ";").count()
                dbr.append("Hemos borrado %d filas de customers"%res)

                res=session.query("select count(*) from customers where customerid="+str(customerid)+";").one()
                dbr.append("Ahora en customers no hay filas para este usuario: "+str(res[0]))

                res=session.query("DELETE from orders where customerid="+ str(customerid) + ";").count()
                dbr.append("Hemos borrado %d filas de orders"%res)

                res=session.query("select count(*) from orders where customerid="+str(customerid)+";").one()
                dbr.append("Ahora en orders no hay filas para este usuario: "+str(res[0]))

            else:
                dbr.append("Vamos a ejecutar la transaccion en orden correcto (orderdetail, orders, customers)")

                res=session.query("DELETE from orderdetail where orderid in (SELECT orderid from orders where customerid=" + str(customerid) + ");").count()
                dbr.append("Hemos borrado %d filas de orderdetail"%res)

                res=session.query("select count(*) from orderdetail where orderid in (select orderid from orders where customerid="+str(customerid)+");").one()
                dbr.append("Ahora en orderdetail no hay filas para este usuario: "+str(res[0]))

                res=session.query("DELETE from orders where customerid="+ str(customerid) + ";").count()

                dbr.append("Hemos borrado %d filas de orders"%res)
                res=session.query("select count(*) from orders where customerid="+str(customerid)+";").one()
                dbr.append("Ahora en orders no hay filas para este usuario: "+str(res[0]))

                res=session.query("DELETE from customers where customerid="+str(customerid) + ";").count()

                dbr.append("Hemos borrado %d filas de customers"%res)
                res=session.query("select count(*) from customers where customerid="+str(customerid)+";").one()
                dbr.append("Ahora en customers no hay filas para este usuario: "+str(res[0]))

    except Exception as e:
        dbr.append("Ha habido un error, vamos a hacer rollback")

        if bSQL is True:
            res=db_conn.execute("ROLLBACK;")
            res=db_conn.execute("select count(*) from orderdetail where orderid in (select orderid from orders where customerid="+str(customerid)+");").fetchone()
            dbr.append("Tras el rollback, volvemos a tener las filas en orderdetail: "+str(res[0]))

            res=db_conn.execute("select count(*) from orders where customerid="+str(customerid)+";").fetchone()
            dbr.append("Y también en orders: "+str(res[0]))

            res=db_conn.execute("select count(*) from customers where customerid="+str(customerid)+";").fetchone()
            dbr.append("Y también en customers: "+str(res[0]))

        else:
            session.rollback()
            res=session.query("select count(*) from orderdetail where orderid in (select orderid from orders where customerid="+str(customerid)+");").one()
            dbr.append("Tras el rollback, volvemos a tener las filas en orderdetail: "+str(res[0]))

            res=session.query("select count(*) from orders where customerid="+str(customerid)+";").one()
            dbr.append("Y también en orders: "+str(res[0]))

            res=session.query("select count(*) from customers where customerid="+str(customerid)+";").one()
            dbr.append("Y también en customers: "+str(res[0]))


        session.close()
        db_conn.close()

        return dbr

    else:
        # TODO: confirmar cambios si todo va bien
        print("Todo ha ido bien, vamos a commitear")
        dbr.append("Todo ha ido bien, vamos a commitear")

        if bSQL is True:
            res=db_conn.execute("COMMIT;")
        else:
            session.commit()

    session.close()
    db_conn.close()

    return dbr
