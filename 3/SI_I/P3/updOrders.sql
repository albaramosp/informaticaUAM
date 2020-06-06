drop function updateOrders;
CREATE OR REPLACE FUNCTION updateOrders()
RETURNS TRIGGER
AS $$
BEGIN
	IF (TG_OP = 'DELETE') THEN
		update orders
		set totalAmount = round(suma*(1+tax/100),2), netAmount = suma
		from (
			select orderid, sum(price) as suma 
			from orderdetail
			where orderid = old.orderid
			group by orderid
		) as s
		where orders.orderid = old.orderid;
		RETURN old;
	ELSIF (TG_OP = 'UPDATE' or TG_OP = 'INSERT') THEN
		update orders
		set totalAmount = round(suma*(1+tax/100),2), netAmount = suma
		from (
			select orderid, sum(price) as suma 
			from orderdetail
			where orderid = new.orderid
			group by orderid
		) as s
		where orders.orderid = new.orderid;
		RETURN new;
	END IF;
	RETURN new;
END;
$$
LANGUAGE 'plpgsql';

drop trigger updOrders on orderdetail;
CREATE TRIGGER updOrders after INSERT or UPDATE or DELETE ON orderdetail
FOR EACH ROW 
EXECUTE PROCEDURE updateOrders();
