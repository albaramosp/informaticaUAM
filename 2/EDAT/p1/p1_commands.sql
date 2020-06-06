---------------------------------------
--Fichero: p1_small.sql              --
--Autores: Alba Ramos, Miguel Díaz   --
--Asignatura: EDAT (prácticas)       --
--Grupo: 1211                        --
---------------------------------------


CREATE DATABASE p1_tweet;
-----------------------------------------------
CREATE TABLE public.follows
(
  id_seguidor integer NOT NULL,
  id_seguido integer NOT NULL,
  CONSTRAINT follows_pkey PRIMARY KEY (id_seguidor, id_seguido)
);

--------------------------------------------------------
CREATE TABLE public.usuarios
(
  id integer NOT NULL,
  nombre character varying(50),
  fecha_registro date,
  CONSTRAINT usuarios_pkey PRIMARY KEY (id)
);
-----------------------------------------------------
CREATE TABLE public.tweets
(
  id integer NOT NULL,
  texto character varying(250),
  localizacion character varying(50),
  fecha date,
  id_autor integer,
  CONSTRAINT tweets_pkey PRIMARY KEY (id)
);
----------------------------------------------
CREATE TABLE public.retweets
(
  id_original varchar(50) NOT NULL,
  id_tweet integer NOT NULL,
  CONSTRAINT retweets_pkey PRIMARY KEY (id_tweet, id_original)
);
  -------------------------------------
  create table follows_temp (
	follower_id int,
	follower_name varchar (50),
	followee_id int,
	followee_name varchar(50),

	primary key(follower_id, followee_id)
);
----------------------------------------
  create table tweets_temp (
	  id_usuario int,
	  nombre_usuario varchar(50),
	  localizacion varchar(50),
	  fecha_registro_usuario date,
	  id_tweet int primary key,
	  fecha_tweet date,
	  texto varchar(250),
	  id_original varchar(50)

);
-----------------------------------------
--A CONTINUACION SE HA IMPORTADO LA TABLA FOLLOWS PEQUEÑA UTILIZANDO IMPORT Y SELECCIONANDO
--EL TAB COMO SEPARADOR
----------------------------------------
----------------------------------------
--A CONTINUACION SE HA IMPORTADO LA TABLA TWEETS PEQUEÑA UTILIZANDO IMPORT Y SELECCIONANDO
--EL TAB COMO SEPARADOR
----------------------------------------

--rellenamos la tabla follows
insert into follows (id_seguidor, id_seguido)
select follower_id, followee_id from follows_temp;
-------------------------------------------

--Rellenamos la tabla usuarios--
--primero insertamos todos los usuarios que hayan tweeteado, con sus fechas de registro
insert into usuarios
(
	select distinct id_usuario, nombre_usuario, fecha_registro_usuario
	from tweets_temp
	order by id_usuario, nombre_usuario, fecha_registro_usuario asc
);

--Después los usuarios seguidores que no hayamos puesto ya...
insert into usuarios (id, nombre)
(select distinct follower_id, follower_name
 from follows_temp
 where follower_id not in
	(
		select id
		from usuarios
	)
);


-- ... y lo mismo con los seguidos
insert into usuarios (id, nombre)
(select distinct followee_id, followee_name
 from follows_temp
 where followee_id not in
	(
		select id
		from usuarios
	)
);
--en estos dos últimos casos, los usuarios aparecen sin fecha de registro


--una vez rellenada la tabla, ahora añadimos las claves foráneas porque si lo haces antes las tablas están vacías y no te lo permite
alter table follows
add foreign  key (id_seguidor) references usuarios(id);

alter table follows
add foreign  key (id_seguido) references usuarios(id);

alter table tweets
add foreign  key (id_autor) references usuarios(id);
--------------------------
--rellenamos la tabla de tweets
insert into tweets
(
	select tweets_temp.id_tweet, tweets_temp.texto, tweets_temp.localizacion, tweets_temp.fecha_tweet, tweets_temp.id_usuario
	from tweets_temp
);

--por ultimo rellenamos la tabla de retweets
insert into retweets
(
	select id_original, id_tweet
	from tweets_temp
	where id_original <>'null'
);

drop table follows;
drop table follows_temp;
drop table retweets;
drop table tweets;
drop table tweets_temp;
drop table usuarios;

--************************************************************************
--************************************************************************
--************************************************************************
--************************************************************************
--************************************************************************
--************************************************************************
--************************************************************************
--************************************************************************


CREATE TABLE public.follows
(
  id_seguidor bigint NOT NULL,
  id_seguido bigint NOT NULL,
  CONSTRAINT follows_pkey PRIMARY KEY (id_seguidor, id_seguido)
);

--------------------------------------------------------
CREATE TABLE public.usuarios
(
  id bigint NOT NULL,
  nombre character varying(200),
  fecha_registro timestamp without time zone,
  CONSTRAINT usuarios_pkey PRIMARY KEY (id)
);
-----------------------------------------------------
CREATE TABLE public.tweets
(
  id bigint NOT NULL,
  texto character varying(250),
  localizacion character varying(200),
  fecha timestamp without time zone,
  id_autor bigint,
  CONSTRAINT tweets_pkey PRIMARY KEY (id)
);
----------------------------------------------
CREATE TABLE public.retweets
(
  id_original varchar(50) NOT NULL,
  id_tweet bigint NOT NULL,
  CONSTRAINT retweets_pkey PRIMARY KEY (id_tweet, id_original)
);
  -------------------------------------
  create table follows_temp (
	follower_id bigint,
	follower_name varchar (200),
	followee_id bigint,
	followee_name varchar(200),

	primary key(follower_id, followee_id)
);
----------------------------------------
  create table tweets_temp (
	  id_usuario bigint,
	  nombre_usuario varchar(200),
	  localizacion varchar(200),
	  fecha_registro_usuario timestamp without time zone,
	  id_tweet bigint primary key,
	  texto varchar(250),
	  fecha_tweet timestamp without time zone,
	  id_original varchar(200)

);
-----------------------------------------
--A CONTINUACION SE HA IMPORTADO LA TABLA FOLLOWS GRANDE UTILIZANDO IMPORT Y SELECCIONANDO
--EL TAB COMO SEPARADOR
----------------------------------------
----------------------------------------
--A CONTINUACION SE HA IMPORTADO LA TABLA TWEETS GRANDE UTILIZANDO IMPORT Y SELECCIONANDO
--EL TAB COMO SEPARADOR
----------------------------------------

--rellenamos la tabla follows
insert into follows (id_seguidor, id_seguido)
select follower_id, followee_id from follows_temp;
-------------------------------------------

--Rellenamos la tabla usuarios--
--primero insertamos todos los usuarios que hayan tweeteado, con sus fechas de registro
insert into usuarios
(
	select distinct id_usuario, nombre_usuario, fecha_registro_usuario
	from tweets_temp
	order by id_usuario, nombre_usuario, fecha_registro_usuario asc
);

--Después los usuarios seguidores que no hayamos puesto ya...
insert into usuarios (id, nombre)
(select distinct follower_id, follower_name
 from follows_temp
 where follower_id not in
	(
		select id
		from usuarios
	)
);


-- ... y lo mismo con los seguidos
insert into usuarios (id, nombre)
(select distinct followee_id, followee_name
 from follows_temp
 where followee_id not in
	(
		select id
		from usuarios
	)
);
--en estos dos últimos casos, los usuarios aparecen sin fecha de registro


--una vez rellenada la tabla, ahora añadimos las claves foráneas porque si lo haces antes las tablas están vacías y no te lo permite
alter table follows
add foreign  key (id_seguidor) references usuarios(id);

alter table follows
add foreign  key (id_seguido) references usuarios(id);

alter table tweets
add foreign  key (id_autor) references usuarios(id);
--------------------------
--rellenamos la tabla de tweets
insert into tweets
(
	select tweets_temp.id_tweet, tweets_temp.texto, tweets_temp.localizacion, tweets_temp.fecha_tweet, tweets_temp.id_usuario
	from tweets_temp
);

--por ultimo rellenamos la tabla de retweets
insert into retweets
(
	select id_original, id_tweet
	from tweets_temp
	where id_original <> 'null'
);