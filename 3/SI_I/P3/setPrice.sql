-- RELLENAMOS EN LA TABLA DE ORDERDETAIL EL CAMPO PRECIO, ESTE LO CALCULAMOS A PARTIR DE LA DIFERENCIA ENTRA LA FECHA ACTUAL Y LA FECHA EN LA QUE SE REALIZO LA ORDEN, CON ESTA DIFERNCIA CALCUMOS EL PORCENTAJE QUE HA AUMENTADO Y SE LO REDUCIMOS AL PRECIO DEL PRODUCTO---

update orderdetail
set price = P.oldprice
from (select orderid,O.prod_id,round(P.price /(1.02^(EXTRACT(YEAR FROM CURRENT_DATE) - EXTRACT(YEAR FROM orderdate))):: numeric, 2) *quantity as oldprice, quantity from orders natural join orderdetail as O  join products as P on P.prod_id = O.prod_id
) as P
where orderdetail.prod_id = P.prod_id and orderdetail.orderid = P.orderid
