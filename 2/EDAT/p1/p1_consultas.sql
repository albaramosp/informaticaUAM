---------------------------------------
--Fichero: p1_consultas.sql              --
--Autores: Alba Ramos, Miguel Díaz   --
--Asignatura: EDAT (prácticas)       --
--Grupo: 1211                        --
---------------------------------------


--a)
--tweet más antiguo;
select *
from tweets
where fecha in (
	select min(fecha)
	from tweets
);
--Tweet más reciente;
-- En este caso, hay 212077 tweets más recientes (misma fecha y hora), pero seleccionamos únicamente 20, como indica el enunciado:
select *
from tweets
where fecha in (
	select max(fecha)
	from tweets
);


--b) densidad de la red, es decir número de seguidores promedio por usuario;
select count(*)/count(distinct id) as promedio
from follows inner join usuarios
on id_seguidor = id;


--d) seguidores comunes a dos usuarios dados: 783214 y 2142731(los usuarios los podéis elegir
--más o menos al azar y poner sus identificadores como constantes en la
--consulta);

--de los 26 resultados, seleccionamos solo los 20 primeros resultados

select distinct id_seguidor
from follows
where id_seguidor in
(

	(
		select id_seguidor
		from follows
		where id_seguido=783214
	)
	intersect
	(
		select id_seguidor
		from follows
		where id_seguido=2142731
	)
);

--e)usuarios comunes seguidos por dos usuarios dados: 13139 y 2929271

select distinct id_seguido
from follows
where id_seguido in
(

	(
		select id_seguido
		from follows
		where id_seguidor=13139
	)
	intersect
	(
		select id_seguido
		from follows
		where id_seguidor=2929271
	)
);


--g) Usuario más seguido

select nombre
from usuarios
where id =
(
	select id_seguido
	from
	(
		select id_seguido, count(*)
		from follows
		group by id_seguido
	)as num_seguidos
	where count = (select max(count) from
	(
		select id_seguido, count(*)
		from follows
		group by id_seguido
	)as num_seguidos
	)
)

--h) Usuario que más tweets ha escrito

select nombre
from usuarios
where id =
(
	select id_autor
	from
	(
		select id_autor, count(*)
		from tweets
		group by id_autor
	)as id
	where count =
	(
		select max(count)
		from
		(
			select id_autor, count(*)
			from tweets
			group by id_autor
		)as autor_max
	)
)
