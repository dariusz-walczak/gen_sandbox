import enum


class AppError(Exception):
    Codes = enum.Enum("Codes", ("InvalidInput",))

    def __init__(self, code, message):
        super().__init__(f"{code.name}: {message}")
        self.code = code

