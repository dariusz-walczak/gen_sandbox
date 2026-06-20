import enum

class Format(enum.Enum):
    HUMAN = enum.auto()
    MACHINE = enum.auto()
    SYMBOLIC = enum.auto()

    def __str__(self):
        return self.name
