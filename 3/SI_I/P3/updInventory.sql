drop function updateInventory;
CREATE OR REPLACE FUNCTION updateInventory()
RETURNS TRIGGER
AS $$
BEGIN
	IF old.status is null and new.status is not null  THEN
	 UPDATE orders as O set netamount = suma , totalamount = round(suma*(1+tax/100),2)
     from (select orderid, sum(price) as suma from orderdetail
     group by orderid ) as S 
	 where O.orderid = S.orderid
     and netamount IS NULL and totalamount IS NULL;
	 
	 update products
	 set stock = stock - s.quantity
	 from (
		 select *
		 from orderdetail
		 where orderdetail.orderid = new.orderid
	 ) as s
	 where s.prod_id = products.prod_id;
	 
	 
	 insert into alertas(prod_id)
	 select s.prod_id
	 from (
		 select products.prod_id,products.stock
		 from orderdetail  join products on orderdetail.prod_id = products.prod_id
		 where orderdetail.orderid = new.orderid
	 ) as s
	 where s.stock = 0;
	 
	 END IF;
	RETURN new;	 
END;
$$
LANGUAGE 'plpgsql';

drop trigger updInventory on orders;
CREATE TRIGGER updInventory after UPDATE ON orders
FOR EACH ROW 
EXECUTE PROCEDURE updateInventory();
