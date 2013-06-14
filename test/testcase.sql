.echo on

select ext_utf8(ext_gbk('²âÊÔ±àÂë×ª»»')) = '²âÊÔ±àÂë×ª»»';

select hex(ext_geo_wkb('POINT(111.1 222.2)')) = '01010000006666666666C65B406666666666C66B40';
select hex(ext_geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = '0102000000020000006666666666C65B406666666666C66B40CDCCCCCCCCD474406666666666C67B40';
select hex(ext_geo_wkb('POLYGON((111.1 222.2,333.3 444.4,555.5 666.6,111.1 222.2))')) = '010300000001000000040000006666666666C65B406666666666C66B40CDCCCCCCCCD474406666666666C67B4000000000005C8140CDCCCCCCCCD484406666666666C65B406666666666C66B40';
select hex(ext_geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = '01040000000200000001010000006666666666C65B406666666666C66B400101000000CDCCCCCCCCD474406666666666C67B40';

select ext_geo_type(ext_geo_wkb('POINT(111.1 222.2)')) = 'Point';
select ext_geo_type(ext_geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = 'LineString';
select ext_geo_type(ext_geo_wkb('POLYGON((111.1 222.2,333.3 444.4,555.5 666.6,111.1 222.2))')) = 'Polygon';
select ext_geo_type(ext_geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 'MultiPoint';

select ext_geo_minx(ext_geo_wkb('POINT(111.1 222.2)')) = 111.1;
select ext_geo_maxx(ext_geo_wkb('POINT(111.1 222.2)')) = 111.1;
select ext_geo_miny(ext_geo_wkb('POINT(111.1 222.2)')) = 222.2;
select ext_geo_maxy(ext_geo_wkb('POINT(111.1 222.2)')) = 222.2;

select ext_geo_minx(ext_geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = 111.1;
select ext_geo_maxx(ext_geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = 333.3;
select ext_geo_miny(ext_geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = 222.2;
select ext_geo_maxy(ext_geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = 444.4;

select ext_geo_minx(ext_geo_wkb('POLYGON((111.1 222.2,333.3 444.4,555.5 666.6,111.1 222.2))')) = 111.1;
select ext_geo_maxx(ext_geo_wkb('POLYGON((111.1 222.2,333.3 444.4,555.5 666.6,111.1 222.2))')) = 555.5;
select ext_geo_miny(ext_geo_wkb('POLYGON((111.1 222.2,333.3 444.4,555.5 666.6,111.1 222.2))')) = 222.2;
select ext_geo_maxy(ext_geo_wkb('POLYGON((111.1 222.2,333.3 444.4,555.5 666.6,111.1 222.2))')) = 666.6;

select ext_geo_minx(ext_geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 111.1;
select ext_geo_maxx(ext_geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 333.3;
select ext_geo_miny(ext_geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 222.2;
select ext_geo_maxy(ext_geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 444.4;

select ext_geo_points(ext_geo_wkb('POINT(111.1 222.2)')) = 1;
select ext_geo_points(ext_geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = 2;
select ext_geo_points(ext_geo_wkb('POLYGON((111.1 222.2,333.3 444.4,555.5 666.6,111.1 222.2))')) = 4;
select ext_geo_points(ext_geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 2;

select ext_geo_area(ext_geo_wkb('POINT(111.1 222.2)')) = 0;
select ext_geo_area(ext_geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = 0;
select ext_geo_area(ext_geo_wkb('POLYGON((0 0,2 0,2 1,0 0))')) = 1.0;
select ext_geo_area(ext_geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 0;

select ext_geo_length(ext_geo_wkb('POINT(111.1 222.2)')) = 0;
select ext_geo_length(ext_geo_wkb('LINESTRING(0 0,2 0,2 1)')) = 3;
select ext_geo_length(ext_geo_wkb('POLYGON((0 0,2 0,2 1,0 0))')) = 5.23606797749979;
select ext_geo_length(ext_geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 0;

select ext_geo_empty(ext_geo_wkb('POINT(111.1 222.2)')) = 0;
select ext_geo_empty(ext_geo_wkb('LINESTRING(0 0,2 0,2 1)')) = 0;
select ext_geo_empty(ext_geo_wkb('POLYGON((0 0,2 0,2 1,0 0))')) = 0;
select ext_geo_empty(ext_geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 0;

select ext_geo_valid(ext_geo_wkb('POINT(111.1 222.2)')) = 1;
select ext_geo_valid(ext_geo_wkb('LINESTRING(0 0,2 0,2 1)')) = 1;
select ext_geo_valid(ext_geo_wkb('POLYGON((0 0,2 0,2 1,0 0))')) = 1;
select ext_geo_valid(ext_geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 1;

select ext_geo_subgeos(ext_geo_wkb('POINT(111.1 222.2)')) = 1;
select ext_geo_subgeos(ext_geo_wkb('LINESTRING(0 0,2 0,2 1)')) = 1;
select ext_geo_subgeos(ext_geo_wkb('POLYGON((0 0,2 0,2 1,0 0))')) = 1;
select ext_geo_subgeos(ext_geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 2;

select ext_geo_x(ext_geo_wkb('POINT(111.1 222.2)')) = 111.1;
select ext_geo_y(ext_geo_wkb('POINT(111.1 222.2)')) = 222.2;





