class Watcher():    
    def get_enum_name(self, value, enum):
        return next((enum_value.name for enum_value in enum if enum_value.value == value), "")