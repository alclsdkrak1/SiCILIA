#user in remote server is
#username:password @ hostname

# will run
# show databases;
# use database-name;
# list tables;
# describe table-name;
# create table todayIs (
#		ID int not null auto increment,
#		Time time,
#		Ambient_Temp float,
#		Target_Temp float,
#		Clo float)
# insert into todayIs ( data gathered from SiCILIA )

import MySQLdb
from datetime import datetime

todayIs = str(datetime.now().date())
todayIs = ''.join(w for w in todayIs.split("-"))
todayIs = 's' + todayIs

print todayIs

db = MySQLdb.connect("hostname","testuser","testpass","testdb")

cursor = db.cursor()

cursor.execute("drop table if exists " + str(todayIs))

sql = "create table " + todayIs
sql = sql + """ (
		id int not null primary key auto_increment,
		t0 time,
		tempS float,
		tempT float,
		clo float)
		"""
print sql
cursor.execute(sql)

theValue = ['175202', 28.04, 31.02, 0.6]
print theValue

sql = "insert into " + todayIs
sql = sql + """(
		t0, tempS, tempT, clo) values (
		"""
sql = sql + str(theValue[0]) + ","
sql = sql + str(theValue[1]) + ","
sql = sql + str(theValue[2]) + ","
sql = sql + str(theValue[3]) + ")"

print sql

cursor.execute(sql)


theValue = ['175401', 22.04, 29.02, 0.2]
print theValue

sql = "insert into " + todayIs
sql = sql + """(
		t0, tempS, tempT, clo) values (
		"""
sql = sql + str(theValue[0]) + ","
sql = sql + str(theValue[1]) + ","
sql = sql + str(theValue[2]) + ","
sql = sql + str(theValue[3]) + ")"

print sql

cursor.execute(sql)
db.commit()

db.close()

