
drop function getTopMonths;
CREATE OR REPLACE FUNCTION getTopMonths (minQuantity bigint, minTotalAmount numeric)
  RETURNS TABLE (
	   año double precision,
     mes double precision,
     productos bigint,
     importe numeric)
AS $$
BEGIN
  RETURN QUERY
  select extract(year from orderdate) as año,
         extract(month from orderdate) as mes,
         sum(quantity) as productos,
         sum(totalamount) as totalamount
  from orders inner join orderdetail
  on orders.orderid = orderdetail.orderid
  group by año, mes
  having sum(quantity) > minQuantity or sum(totalamount) > minTotalAmount;
END; $$
LANGUAGE 'plpgsql';

select * from getTopMonths (19000, 2100000)
