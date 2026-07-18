import enum


class AppError(Exception):
    Codes = enum.Enum("Codes", ("InvalidInput", "InternalError"))

    def __init__(self, code: "AppError.Codes", message: str) -> None:
        super().__init__(f"{code.name}: {message}")
        self.code = code

