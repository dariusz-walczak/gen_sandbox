import enum

class Format(enum.Enum):
    HUMAN = enum.auto() # Human-friendly, colored console output
    MACHINE = enum.auto() # JSON
    SYMBOLIC = enum.auto() # Symbolic expression
    CONTEXT = enum.auto() # Generative AI friendly plain text

    def __str__(self):
        return self.name
