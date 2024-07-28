class_name Message
extends Resource


var sender_id: String
var data: Variant


# Construct message
# @param sender_id sender id
# @param message message
func _init(sender_id: String, data: Variant):
	self.sender_id = sender_id
	self.data = data
