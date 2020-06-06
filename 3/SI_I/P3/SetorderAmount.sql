CREATE OR REPLACE PROCEDURE setOrderAmount()
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE orders as O set netamount = suma , totalamount = round(suma*(1+tax/100),2)
     from (select orderid, sum(price) as suma from orderdetail
     group by orderid ) as S where O.orderid = S.orderid
     and netamount IS NULL and totalamount IS NULL;
    COMMIT;
END;
$$;

call setOrderAmount();
