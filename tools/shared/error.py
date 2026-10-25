import enum


class AppError(Exception):
    Codes = enum.Enum("Codes", (
        "InvalidInput",
        "InternalError",
        # Indicates function input contract violations (internal in nature):
        "InputContractError",
        # Indicates an error made by the application user (e.g. providing contradictory input
        #  data):
        "UserError",
    ))

    def __init__(self, code: "AppError.Codes", message: str) -> None:
        super().__init__(f"{code.name}: {message}")
        self.code = code
