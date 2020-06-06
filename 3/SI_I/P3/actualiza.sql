--psql -U alumnodb -d si1 -f dump_v1.3.sql

-----------------------------------------------------------------------ORDERS-------------------------------------------------------
-- AÑADIMOS CLAVE EXTERNA HACIA CUSTOMERS
ALTER TABLE ONLY public.orders
    ADD CONSTRAINT imdb_orders_customerid_fkey
	FOREIGN KEY (customerid)
	REFERENCES public.customers(customerid)
	ON DELETE CASCADE
	ON UPDATE CASCADE;

-----------------------------------------------------------------------IMDB_ACTORMOVIES-------------------------------------------------------
-- MODIFICAMOS LA TABLA DE ACTORES Y PELICULAS AÑADIENDO CLAVES EXTERNAS CON BORRADO Y ACTUALIZACION EN CASCADA.
-- AÑADIDA CLAVE PRIMARIA
ALTER TABLE ONLY public.imdb_actormovies
    ADD CONSTRAINT imdb_actormovies_actorid_fkey FOREIGN KEY (actorid)
	REFERENCES public.imdb_actors(actorid)
	ON DELETE CASCADE
	ON UPDATE CASCADE;

ALTER TABLE ONLY public.imdb_actormovies
    ADD CONSTRAINT imdb_actormovies_movieid_fkey
	FOREIGN KEY (movieid)
	REFERENCES public.imdb_movies(movieid)
	ON DELETE CASCADE
	ON UPDATE CASCADE;

ALTER TABLE ONLY public.imdb_actormovies
    ADD CONSTRAINT actormovies_pkey PRIMARY KEY (actorid, movieid);



-----------------------------------------------------------------------IMDB_DIRECTORMOVIES---------------------------------------------------------
-- MODIFICAMOS LA TABLA DE DIRECTORES Y PELICULAS MODIFICANDO BORRADO Y ACTUALIZACION EN CASCADA
-- SOBRE LAS CLAVES EXTERNAS.
-- ELIMINADO CAMPO PARTICIPATION DE LA CLAVE PRIMARIA
ALTER TABLE ONLY public.imdb_directormovies
	DROP CONSTRAINT imdb_directormovies_directorid_fkey,
	DROP CONSTRAINT imdb_directormovies_movieid_fkey,
	DROP CONSTRAINT imdb_directormovies_pkey;

ALTER TABLE ONLY public.imdb_directormovies
    ADD CONSTRAINT imdb_directormovies_pkey PRIMARY KEY (directorid, movieid);

ALTER TABLE ONLY public.imdb_directormovies
    ADD CONSTRAINT imdb_directormovies_directorid_fkey
	FOREIGN KEY (directorid)
	REFERENCES public.imdb_directors(directorid)
	ON DELETE CASCADE
	ON UPDATE CASCADE;

ALTER TABLE ONLY public.imdb_directormovies
    ADD CONSTRAINT imdb_directormovies_movieid_fkey
	FOREIGN KEY (movieid)
	REFERENCES public.imdb_movies(movieid)
	ON DELETE CASCADE
	ON UPDATE CASCADE;

-- ELIMINAMOS EL AUTOINCREMENTADO DEL ID DEL DIRECTOR Y DE LA PELÍCULA
ALTER TABLE ONLY public.imdb_directormovies
	alter column movieid
	DROP default;
DROP sequence imdb_directormovies_movieid_seq;

ALTER TABLE ONLY public.imdb_directormovies
    alter column directorid
    DROP default;

DROP sequence imdb_directormovies_directorid_seq;


-----------------------------------------------------------------------PRODUCTS----------------------------------------------------------------
-- MODIFICAMOS LA TABLA DE PRODUCTOS MODIFICANDO BORRADO Y ACTUALIZACION EN CASCADA
-- SOBRE LA CLAVE EXTERNA.
-- AÑADIDOS CAMPOS DEL INVENTARIO.
ALTER TABLE PRODUCTS
    ADD sales integer NOT NULL DEFAULT 0;

ALTER TABLE PRODUCTS
	ADD	stock integer NOT NULL DEFAULT 0;

COMMENT ON COLUMN public.products.stock IS 'quantity in stock';
COMMENT ON COLUMN public.products.sales IS 'quantity sold';

ALTER TABLE ONLY public.products
	DROP CONSTRAINT products_movieid_fkey;

ALTER TABLE ONLY public.products
    ADD CONSTRAINT products_movieid_fkey
	FOREIGN KEY (movieid)
	REFERENCES public.imdb_movies(movieid)
	ON DELETE CASCADE
	ON UPDATE CASCADE;


-- RELLENAMOS EN LA TABLA DE PRODUCTOS LOS NUEVOS CAMPOS CON LA INFORMACION DEL INVENTARIO.
update products
set sales = inventory.sales, stock = inventory.stock
from inventory
where inventory.prod_id = products.prod_id;

-- RELLENAMOS EN LA TABLA DE PRODUCTOS LA INFORMACION DE LAS VENTAS QUE NO APARECEN EN INVENTORY, PERO
-- SI EN ORDERDETAIL. ASUMIMOS QUE EL STOCK ES 0 PARA ESTOS CASOS, QUE SE HAN VENDIDO TODOS LOS EJEMPLARES.
update products
set sales = c.s
from (
	select prod_id, sum(quantity) as s
	from orderdetail
	group by prod_id
) as c
where c.prod_id = products.prod_id
	  and c.prod_id not in (
		  select prod_id
		  from inventory
	  );


-- FINALMENTE, BORRAMOS LA TABLA DE INVENTARIO, PORQUE YA HEMOS GUARDADO ESA INFORMACION
-- EN LOS ATRIBUTOS QUE HEMOS CREADO EN PRODUCTO.
drop table inventory;

-----------------------------------------------------------------------ORDERDETAIL--------------------------------------------------------------
-- EN LA TABLA DE DETALLES DE PEDIDOS:
-- AÑADIMOS CLAVES EXTERNAS CON SUS RESTRICCIONES
ALTER TABLE ONLY public.orderdetail
    ADD CONSTRAINT imdb_orderdetail_orderid_fkey
	FOREIGN KEY (orderid)
	REFERENCES public.orders(orderid)
	ON DELETE CASCADE
	ON UPDATE CASCADE;

ALTER TABLE ONLY public.orderdetail
    ADD CONSTRAINT imdb_orderdetail_prod_id_fkey
	FOREIGN KEY (prod_id)
	REFERENCES public.products(prod_id)
	ON DELETE CASCADE
	ON UPDATE CASCADE;

-- PARA LAS ENTRADAS DUPLICADAS, INCREMENTAMOS LAS CANTIDADES EN LAS ENTRADAS REPETIDAS
-- Esto sirve para que después borremos una de las filas repetidas, y que la que se quede
-- tenga la cantidad actualizada incluyendo la que se borró.
CREATE TABLE public.orderdetail2 (
    orderid integer NOT NULL,
    prod_id integer NOT NULL,
    price numeric,
    quantity integer NOT NULL
);
ALTER TABLE public.orderdetail2 OWNER TO alumnodb;
COMMENT ON COLUMN public.orderdetail2.price IS 'price without taxes when the order was paid';


insert into orderdetail2 (orderid, prod_id, quantity)
	select orderid, prod_id, sum(quantity)
	from orderdetail
	group by orderid, prod_id;

delete from orderdetail;

insert into orderdetail
	select * from orderdetail2;

drop table orderdetail2;

-- YA PODEMOS AÑADIR LA RESTRICCIÓN DE CLAVE PRIMARIA
ALTER TABLE ONLY public.orderdetail
    ADD CONSTRAINT imdb_orderdetail_pkey PRIMARY KEY (orderid, prod_id);

-- SE ACTUALIZA EL PRECIO DEL PEDIDO USANDO EL PRECIO DEL PRODUCTO MULTIPLICADO POR LA CANTIDAD
--update orderdetail
--set price = products.price*quantity
--from products
--where products.prod_id = orderdetail.prod_id


-----------------------------------------------------------------------LANGUAGES--------------------------------------------------------------
-- CREAMOS LA TABLA DE LENGUAJES, SE LA ASOCIAMOS A ALUMNODB Y CREAMOS UNA SECUENCIA PARA EL ID AUTOINCREMENTADO
CREATE TABLE languages (
    lang_id int NOT NULL,
    language varchar(255) NOT NULL
);

ALTER TABLE public.languages OWNER TO alumnodb;

CREATE SEQUENCE public.languages_lang_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER TABLE public.languages_lang_id_seq OWNER TO alumnodb;
ALTER SEQUENCE public.languages_lang_id_seq OWNED BY public.languages.lang_id;
ALTER TABLE ONLY public.languages ALTER COLUMN lang_id SET DEFAULT nextval('public.languages_lang_id_seq'::regclass);
ALTER TABLE ONLY public.languages ADD CONSTRAINT languages_pkey PRIMARY KEY (lang_id);

-- RELLENAMOS LA TABLA DE LENGUAJES A PARTIR DE LOS EXISTENTES EN LA TABLA QUE NOS DAN
INSERT INTO LANGUAGES(language)
	SELECT DISTINCT LANGUAGE FROM IMDB_MOVIELANGUAGES;


-----------------------------------------------------------------------IMDB_MOVIELANGUAGES--------------------------------------------------------
-- CREAMOS UNA COLUMNA PARA GUARDAR EL ID DEL LENGUAJE DE LA PELICULA
-- SERA UNA CLAVE EXTERNA HACIA LA TABLA DE LENGUAJES QUE HEMOS CREADO
-- PONDREMOS UN VALOR EXISTENTE POR DEFECTO PARA NO VIOLAR LA RESTRICCION DE NULIDAD
alter table imdb_movielanguages
add column lang_id integer not null default 1;

ALTER TABLE ONLY public.imdb_movielanguages
    ADD CONSTRAINT imdb_movielanguages_lang_id_fkey
	FOREIGN KEY (lang_id)
	REFERENCES public.languages(lang_id)
	ON DELETE CASCADE
	ON UPDATE CASCADE;

-- ACTUALIZAMOS LOS IDS DE LENGUAJE A SU VALOR CORRECTO (EL DE LA TABLA DE LENGUAJES)
update imdb_movielanguages
set lang_id = languages.lang_id
from languages
where languages.language = imdb_movielanguages.language;

-- PODEMOS BORRAR LA COLUMNA CON EL NOMBRE DEL LENGUAJE, PUES YA TENEMOS LA CLAVE EXTERNA CON EL ID
alter table imdb_movielanguages drop column language;

-- AÑADIMOS LA CLAVE PRIMARIA DE NUEVO, PUES AL BORRAR LA COLUMNA QUE LA CONTENÍA DESAPARECE
ALTER TABLE ONLY public.imdb_movielanguages
    ADD CONSTRAINT imdb_movielanguages_pkey PRIMARY KEY (movieid, lang_id);

-- Y TAMBIÉN MODIFICAMOS LA CLAVE EXTERNA DEL MOVIE_ID PARA QUE SE BORRE EN CASCADA
ALTER TABLE ONLY public.imdb_movielanguages
	DROP CONSTRAINT imdb_movielanguages_movieid_fkey;

ALTER TABLE ONLY public.imdb_movielanguages
    ADD CONSTRAINT imdb_movielanguages_movieid_fkey
	FOREIGN KEY (movieid)
	REFERENCES public.imdb_movies(movieid)
	ON DELETE CASCADE
	ON UPDATE CASCADE;


-----------------------------------------------------------------------COUNTRIES--------------------------------------------------------------
-- CREAMOS LA TABLA DE PAISES, SE LA ASOCIAMOS A ALUMNODB Y CREAMOS UNA SECUENCIA PARA EL ID AUTOINCREMENTADO
CREATE TABLE countries (
    country_id int NOT NULL,
    country varchar(255) NOT NULL
);

ALTER TABLE public.countries OWNER TO alumnodb;

CREATE SEQUENCE public.countries_country_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER TABLE public.countries_country_id_seq OWNER TO alumnodb;
ALTER SEQUENCE public.countries_country_id_seq OWNED BY public.countries.country_id;
ALTER TABLE ONLY public.countries ALTER COLUMN country_id SET DEFAULT nextval('public.countries_country_id_seq'::regclass);
ALTER TABLE ONLY public.countries ADD CONSTRAINT countries_pkey PRIMARY KEY (country_id);

-- RELLENAMOS LA TABLA DE PAISES A PARTIR DE LOS EXISTENTES EN LA TABLA QUE NOS DAN
INSERT INTO COUNTRIES(country)
	SELECT DISTINCT country FROM IMDB_MOVIECOUNTRIES;

-----------------------------------------------------------------------IMDB_MOVIECOUNTRIES--------------------------------------------------------
-- CREAMOS UNA COLUMNA PARA GUARDAR EL ID DEL PAIS DE LA PELICULA
-- SERA UNA CLAVE EXTERNA HACIA LA TABLA DE PAISES QUE HEMOS CREADO
-- PONDREMOS UN VALOR EXISTENTE POR DEFECTO PARA NO VIOLAR LA RESTRICCION DE NULIDAD
alter table imdb_moviecountries
add column country_id integer not null default 1;

ALTER TABLE ONLY public.imdb_moviecountries
    ADD CONSTRAINT imdb_moviecountries_country_id_fkey
	FOREIGN KEY (country_id)
	REFERENCES public.countries(country_id)
	ON DELETE CASCADE
	ON UPDATE CASCADE;

-- ACTUALIZAMOS LOS IDS DE LENGUAJE A SU VALOR CORRECTO (EL DE LA TABLA DE LENGUAJES)
update imdb_moviecountries
set country_id = countries.country_id
from countries
where countries.country = imdb_moviecountries.country;

-- PODEMOS BORRAR LA COLUMNA CON EL NOMBRE DEL LENGUAJE, PUES YA TENEMOS LA CLAVE EXTERNA CON EL ID
alter table imdb_moviecountries drop column country;

-- AÑADIMOS LA CLAVE PRIMARIA DE NUEVO, PUES AL BORRAR LA COLUMNA QUE LA CONTENÍA DESAPARECE
ALTER TABLE ONLY public.imdb_moviecountries
ADD CONSTRAINT imdb_moviecountries_pkey PRIMARY KEY (movieid, country_id);

-- Y TAMBIÉN MODIFICAMOS LA CLAVE EXTERNA DEL MOVIE_ID PARA QUE SE BORRE EN CASCADA
ALTER TABLE ONLY public.imdb_moviecountries
	DROP CONSTRAINT imdb_moviecountries_movieid_fkey;

ALTER TABLE ONLY public.imdb_moviecountries
    ADD CONSTRAINT imdb_moviecountries_movieid_fkey
	FOREIGN KEY (movieid)
	REFERENCES public.imdb_movies(movieid)
	ON DELETE CASCADE
	ON UPDATE CASCADE;

-- ELIMINAMOS EL AUTOINCREMENTADO DEL ID DE LA PELÍCULA
ALTER TABLE ONLY public.imdb_moviecountries
	alter column movieid
	DROP default;
DROP sequence imdb_moviecountries_movieid_seq;


-----------------------------------------------------------------------GENRES--------------------------------------------------------------
-- CREAMOS LA TABLA DE GENEROS, SE LA ASOCIAMOS A ALUMNODB Y CREAMOS UNA SECUENCIA PARA EL ID AUTOINCREMENTADO
CREATE TABLE genres (
    genre_id int NOT NULL,
    genre varchar(255) NOT NULL
);

ALTER TABLE public.genres OWNER TO alumnodb;

CREATE SEQUENCE public.genres_genre_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER TABLE public.genres_genre_id_seq OWNER TO alumnodb;
ALTER SEQUENCE public.genres_genre_id_seq OWNED BY public.genres.genre_id;
ALTER TABLE ONLY public.genres ALTER COLUMN genre_id SET DEFAULT nextval('public.genres_genre_id_seq'::regclass);
ALTER TABLE ONLY public.genres ADD CONSTRAINT genres_pkey PRIMARY KEY (genre_id);

-- RELLENAMOS LA TABLA DE PAISES A PARTIR DE LOS EXISTENTES EN LA TABLA QUE NOS DAN
INSERT INTO genres(genre)
	SELECT DISTINCT genre FROM IMDB_MOVIEGENRES;

-----------------------------------------------------------------------IMDB_MOVIEGENRES--------------------------------------------------------
-- CREAMOS UNA COLUMNA PARA GUARDAR EL ID DEL GENERO DE LA PELICULA
-- SERA UNA CLAVE EXTERNA HACIA LA TABLA DE GENEROS QUE HEMOS CREADO
-- PONDREMOS UN VALOR EXISTENTE POR DEFECTO PARA NO VIOLAR LA RESTRICCION DE NULIDAD
alter table imdb_moviegenres
add column genre_id integer not null default 1;

ALTER TABLE ONLY public.imdb_moviegenres
    ADD CONSTRAINT imdb_moviegenres_genre_id_fkey
	FOREIGN KEY (genre_id)
	REFERENCES public.genres(genre_id)
	ON DELETE CASCADE
	ON UPDATE CASCADE;

-- ACTUALIZAMOS LOS IDS DE GENERO A SU VALOR CORRECTO (EL DE LA TABLA DE GENEROS)
update imdb_moviegenres
set genre_id = genres.genre_id
from genres
where genres.genre = imdb_moviegenres.genre;

-- PODEMOS BORRAR LA COLUMNA CON EL NOMBRE DEL GENERO, PUES YA TENEMOS LA CLAVE EXTERNA CON EL ID
alter table imdb_moviegenres drop column genre;

-- AÑADIMOS LA CLAVE PRIMARIA DE NUEVO, PUES AL BORRAR LA COLUMNA QUE LA CONTENÍA DESAPARECE
ALTER TABLE ONLY public.imdb_moviegenres
ADD CONSTRAINT imdb_moviegenres_pkey PRIMARY KEY (movieid, genre_id);

-- Y TAMBIÉN MODIFICAMOS LA CLAVE EXTERNA DEL MOVIE_ID PARA QUE SE BORRE EN CASCADA
ALTER TABLE ONLY public.imdb_moviegenres
	DROP CONSTRAINT imdb_moviegenres_movieid_fkey;

ALTER TABLE ONLY public.imdb_moviegenres
    ADD CONSTRAINT imdb_moviegenres_movieid_fkey
	FOREIGN KEY (movieid)
	REFERENCES public.imdb_movies(movieid)
	ON DELETE CASCADE
	ON UPDATE CASCADE;

-- ELIMINAMOS EL AUTOINCREMENTADO DEL ID DE LA PELÍCULA
ALTER TABLE ONLY public.imdb_moviegenres
	alter column movieid
	DROP default;
DROP sequence imdb_moviegenres_movieid_seq;

-----------------------------------------------------------------------CUSTOMERS--------------------------------------------------------
-- ESTABLECEMOS QUE EL EMAIL NO PUEDA SER NULO Y DEBA SER UNICO, PUES ES UN CAMPO OBLIGATORIO DEL FORMULARIO Y CON EL
-- DETERMINAREMOS SI UN USUARIO YA EXISTE, PUESTO QUE EN LA BASE HAY USERNAMES REPETIDOS
ALTER TABLE customers
  ALTER COLUMN email SET NOT NULL;

alter table customers ADD CONSTRAINT customers_unique_email UNIQUE (email);

-- CREAMOS UNA COLUMNA PARA GUARDAR EL ID DEL PAIS DEL COMPRADOR
alter table customers
add column country_id integer default 1;

-- LA ESTABLECEMOS COMO CLAVE EXTERNA CON SUS RESTRICCIONES
ALTER TABLE ONLY public.customers
    ADD CONSTRAINT customers_country_id_fkey
	FOREIGN KEY (country_id)
	REFERENCES public.countries(country_id)
	ON DELETE CASCADE
	ON UPDATE CASCADE;

-- ACTUALIZAMOS LOS IDS DEL PAIS A SU VALOR CORRECTO (EL DE LA TABLA DE PAISES)
update customers
set country_id = countries.country_id
from countries
where countries.country = customers.country;

-- PODEMOS BORRAR LA COLUMNA CON EL NOMBRE DEL PAIS, PUES YA TENEMOS LA CLAVE EXTERNA CON EL ID
alter table customers
  drop column country;

-- ELIMINAMOS COLUMNAS INNECESARIAS QUE NO TENEMOS EN NUESTRO FORMULARIO DE REGISTRO
alter table customers
	drop column zip;
alter table customers
	drop column city;
alter table customers
	drop column lastname;
alter table customers
	drop column firstname;
alter table customers
	drop column address2;
alter table customers
	drop column address1;
alter table customers
	drop column gender;
alter table customers
	drop column age;
alter table customers
	drop column creditcardexpiration;
alter table customers
	drop column creditcardtype;
alter table customers
	drop column phone;
alter table customers
	drop column region;
alter table customers
	drop column state;

-- SI INSERTAMOS MANUALMENTE EL ID, LA SECUENCIA DE AUTOINCREMENTO QUEDA OBSOLETA.
-- ARREGLAMOS SU VALOR MANUALMENTE HACIENDO QUE TOME EL VALOR DEL MÁXIMO ID QUE HAY EN LA TABLA
SELECT setval(pg_get_serial_sequence('customers', 'customerid'), coalesce(max(customerid)+1,1), false)
FROM customers;

-----------------------------------------------------------------------ALERTAS--------------------------------------------------------
CREATE TABLE alertas (
    prod_id int NOT NULL
);

-- Creamos la tabla de alertas para cuando un producto se queda sin stock
ALTER TABLE public.alertas OWNER TO alumnodb;
ALTER TABLE ONLY public.alertas ADD CONSTRAINT alertas_pkey PRIMARY KEY (prod_id);

ALTER TABLE ONLY public.alertas
    ADD CONSTRAINT alertas_prod_id_fkey
	FOREIGN KEY (prod_id)
	REFERENCES public.products(prod_id)
	ON DELETE CASCADE
	ON UPDATE CASCADE;

--Actualizamos la secuencia de orders para que obtenga el valor correcto de id
SELECT setval(pg_get_serial_sequence('orders', 'orderid'), coalesce(max(orderid)+1,1), false)
FROM orders;

-- Añadimos clave externa hacia la tabla usuarios desde las ordenes
ALTER TABLE ONLY public.orders
    ADD CONSTRAINT orders_customerid_fkey
	FOREIGN KEY (customerid)
	REFERENCES public.customers(customerid)
	ON DELETE CASCADE
	ON UPDATE CASCADE;
