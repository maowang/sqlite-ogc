.echo on

select utf8(gbk('测试编码转换')) = '测试编码转换';

select hex(geo_wkb('POINT(111.1 222.2)')) = '01010000006666666666C65B406666666666C66B40';
select hex(geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = '0102000000020000006666666666C65B406666666666C66B40CDCCCCCCCCD474406666666666C67B40';
select hex(geo_wkb('POLYGON((111.1 222.2,333.3 444.4,555.5 666.6,111.1 222.2))')) = '010300000001000000040000006666666666C65B406666666666C66B40CDCCCCCCCCD474406666666666C67B4000000000005C8140CDCCCCCCCCD484406666666666C65B406666666666C66B40';
select hex(geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = '01040000000200000001010000006666666666C65B406666666666C66B400101000000CDCCCCCCCCD474406666666666C67B40';

select geo_type(geo_wkb('POINT(111.1 222.2)')) = 'Point';
select geo_type(geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = 'LineString';
select geo_type(geo_wkb('POLYGON((111.1 222.2,333.3 444.4,555.5 666.6,111.1 222.2))')) = 'Polygon';
select geo_type(geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 'MultiPoint';

select geo_minx(geo_wkb('POINT(111.1 222.2)')) = 111.1;
select geo_maxx(geo_wkb('POINT(111.1 222.2)')) = 111.1;
select geo_miny(geo_wkb('POINT(111.1 222.2)')) = 222.2;
select geo_maxy(geo_wkb('POINT(111.1 222.2)')) = 222.2;

select geo_minx(geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = 111.1;
select geo_maxx(geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = 333.3;
select geo_miny(geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = 222.2;
select geo_maxy(geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = 444.4;

select geo_minx(geo_wkb('POLYGON((111.1 222.2,333.3 444.4,555.5 666.6,111.1 222.2))')) = 111.1;
select geo_maxx(geo_wkb('POLYGON((111.1 222.2,333.3 444.4,555.5 666.6,111.1 222.2))')) = 555.5;
select geo_miny(geo_wkb('POLYGON((111.1 222.2,333.3 444.4,555.5 666.6,111.1 222.2))')) = 222.2;
select geo_maxy(geo_wkb('POLYGON((111.1 222.2,333.3 444.4,555.5 666.6,111.1 222.2))')) = 666.6;

select geo_minx(geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 111.1;
select geo_maxx(geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 333.3;
select geo_miny(geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 222.2;
select geo_maxy(geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 444.4;

select geo_points(geo_wkb('POINT(111.1 222.2)')) = 1;
select geo_points(geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = 2;
select geo_points(geo_wkb('POLYGON((111.1 222.2,333.3 444.4,555.5 666.6,111.1 222.2))')) = 4;
select geo_points(geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 2;

select geo_area(geo_wkb('POINT(111.1 222.2)')) = 0;
select geo_area(geo_wkb('LINESTRING(111.1 222.2,333.3 444.4)')) = 0;
select geo_area(geo_wkb('POLYGON((0 0,2 0,2 1,0 0))')) = 1.0;
select geo_area(geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 0;

select geo_length(geo_wkb('POINT(111.1 222.2)')) = 0;
select geo_length(geo_wkb('LINESTRING(0 0,2 0,2 1)')) = 3;
select geo_length(geo_wkb('POLYGON((0 0,2 0,2 1,0 0))')) = 5.23606797749979;
select geo_length(geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 0;

select geo_empty(geo_wkb('POINT(111.1 222.2)')) = 0;
select geo_empty(geo_wkb('LINESTRING(0 0,2 0,2 1)')) = 0;
select geo_empty(geo_wkb('POLYGON((0 0,2 0,2 1,0 0))')) = 0;
select geo_empty(geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 0;

select geo_valid(geo_wkb('POINT(111.1 222.2)')) = 1;
select geo_valid(geo_wkb('LINESTRING(0 0,2 0,2 1)')) = 1;
select geo_valid(geo_wkb('POLYGON((0 0,2 0,2 1,0 0))')) = 1;
select geo_valid(geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 1;

select geo_subgeos(geo_wkb('POINT(111.1 222.2)')) = 1;
select geo_subgeos(geo_wkb('LINESTRING(0 0,2 0,2 1)')) = 1;
select geo_subgeos(geo_wkb('POLYGON((0 0,2 0,2 1,0 0))')) = 1;
select geo_subgeos(geo_wkb('MULTIPOINT((111.1 222.2),(333.3 444.4))')) = 2;

select geo_x(geo_wkb('POINT(111.1 222.2)')) = 111.1;
select geo_y(geo_wkb('POINT(111.1 222.2)')) = 222.2;

select geo_simplify(geo_wkb('LINESTRING(100 100,200 200,300 300,400 400)'),0) = geo_wkb('LINESTRING(100 100,400 400)');
select geo_intersection(geo_wkb('LINESTRING(0 0,100 100)'),geo_wkb('LINESTRING(100 0,0 100)')) = geo_wkb('POINT(50 50)');
select geo_intersects(geo_wkb('LINESTRING(0 0,100 100)'),geo_wkb('LINESTRING(100 0,0 100)'));
select geo_intersects(geo_wkb('LINESTRING(0 0,100 100)'),geo_wkb('LINESTRING(100 0,200 100)')) = 0;