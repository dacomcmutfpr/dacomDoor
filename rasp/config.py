URL_SERVER = "http://192.168.237.21:5000"
ROOM_NAME = "E003"
ROOM_LAST_UPDATE_FAKE = "2001-01-01T00:00:00+00:00"

# SQLAlchemy
SQLALCHEMY_ECHO = False
SQLALCHEMY_TRACK_MODIFICATIONS = True

# MYSQL - TROCAR NOMES
DB_HOST = "localhost"
DB_SCHEMA = "dacomDoor"
DB_USERNAME = "user"
DB_PASSWORD = "user"

# MySQL + SQLAlchemy
SQLALCHEMY_DATABASE_URI = ("mysql+pymysql://{DB_USER}:{DB_PASS}@{DB_ADDR}/{DB_NAME}"
							.format(DB_USER=DB_USERNAME, DB_PASS=DB_PASSWORD,
									DB_ADDR=DB_HOST, DB_NAME=DB_SCHEMA))
