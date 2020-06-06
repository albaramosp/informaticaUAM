--Esta consulta recupera todos los tweets, ordenados cronológicamente, del usuario más antiguo de la base de datos.

select *
from usuarios, tweets
where usuarios.id = id_autor
	and fecha_registro in (
		select min(fecha_registro)
		from usuarios
	)
	order by (fecha) asc


	-- Esta consulta busca el usuario que más retweets ha hecho

	select nombre
	from usuarios
	where id =
	(
		select id_autor
		from
		(
			select id_autor, count(*)
			from
			(
				select tweets.id_autor, retweets.id_tweet
				from tweets
				join retweets on tweets.id = retweets.id_tweet
			)as usuariosr
			group by id_autor
	)as usuariosr
	where count =
	(
		select max(count) from
		(
			select id_autor, count(*)
			from
			(
				select tweets.id_autor, retweets.id_tweet
				from tweets
				join retweets on tweets.id = retweets.id_tweet
			)as usuariosr
			group by id_autor
		)as rec
	)
)
