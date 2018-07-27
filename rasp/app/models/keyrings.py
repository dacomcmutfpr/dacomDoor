from marshmallow import Schema, fields

from app.models.base import db, CRUD
from app.models.enums import UserTypesEnum, UserTypeField


class Keyring(db.Model, CRUD):
	__tablename__ = "keyrings"

	id = db.Column(db.Integer, primary_key=True)
	uid = db.Column(db.String(16), nullable=False, unique=True)
	userId = db.Column(db.Integer, nullable=False)
	userType = db.Column(db.Enum(UserTypesEnum), nullable=False)
	lastUpdate = db.Column(db.DateTime(), nullable=False)

	def __init__(self, uid, userId, userType, lastUpdate):
		self.uid = uid
		self.userId = userId
		self.userType = userType
		self.lastUpdate = lastUpdate


class KeyringSchema(Schema):
	uid = fields.String()
	userId = fields.Integer()
	userType = UserTypeField()
	lastUpdate = fields.DateTime()
