alter table customers add column promo decimal(4,2) default 0;

CREATE OR REPLACE FUNCTION aplicarPromo()
RETURNS TRIGGER
AS $$
BEGIN
	PERFORM pg_sleep(5);
	update orders
	set totalamount=(totalamount/((100-old.promo)/100))*((100-new.promo)/100)
	where customerid = new.customerid;

	RETURN new;
END;
$$
LANGUAGE 'plpgsql';

DROP TRIGGER IF EXISTS promoEnCarrito ON customers;
CREATE TRIGGER promoEnCarrito after UPDATE of promo ON customers
FOR EACH ROW
EXECUTE PROCEDURE aplicarPromo();

begin;
update customers set promo=3 where customerid=98;
commit;

















------------------------------------------------------------------------------------------------------------------
alter table customers add column promo decimal(4,2) default 0;
alter table customers drop column promo

select * from orders where orderid = 113079;

select promo from customers where customerid=8747

update customers set promo=10 where customerid=8747

CREATE OR REPLACE FUNCTION aplicarPromo()
RETURNS TRIGGER
AS $$
BEGIN
	update orders
	set totalamount=(totalamount/((100-old.promo)/100))*((100-new.promo)/100)
	where customerid = new.customerid;

	RETURN new;
END;
$$
LANGUAGE 'plpgsql';

DROP TRIGGER IF EXISTS promoEnCarrito ON customers;
CREATE TRIGGER promoEnCarrito after UPDATE of promo ON customers
FOR EACH ROW
EXECUTE PROCEDURE aplicarPromo();
