create index idx_totalamount on orders(totalamount);

explain analyze select count (distinct orders.customerid)
from customers inner join orders
on orders.customerid = customers.customerid
where extract(year from orderdate)=2015 and
	  extract(month from orderdate)=04 and
	  totalamount>100;

select count (distinct orders.customerid)
from customers inner join orders
on orders.customerid = customers.customerid
where extract(year from orderdate)=2015 and
	  extract(month from orderdate)=04 and
	  totalamount>100;
