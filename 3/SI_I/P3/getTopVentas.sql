drop function getTopVentas;
CREATE OR REPLACE FUNCTION getTopVentas (n INT) 
	RETURNS TABLE (
		año double precision,
		pelicula VARCHAR,
		ventas BIGINT )AS $$ 
DECLARE
BEGIN
    loop
	exit when n > extract(year from now());
	
	return query
	select extract(year from orderdate) as año, movietitle, sum(sales) as sales
	from orderdetail, products, imdb_movies, orders
	where orderdetail.prod_id = products.prod_id
		  and products.movieid = imdb_movies.movieid
		  and orders.orderid = orderdetail.orderid
		  and extract(year from orderdate) = n
	group by movietitle, año
	order by sales desc
	limit 1;
	n := n+1;
end loop;
END ; 
$$ LANGUAGE plpgsql;

select * from getTopVentas(2014);

