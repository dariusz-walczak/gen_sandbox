import pydantic


def default_cb(obj: object) -> object:
    if isinstance(obj, pydantic.BaseModel):
        return obj.model_dump()
    raise TypeError(f"Object of type {type(obj).__name__} is not JSON serializable")
