class EmptyDataError(Exception):
    """
    Gets thrown when the Data is of size 0, and therefore nothing
    else can be done.
    """
    pass


class EmptyCentroidsError(Exception):
    """
    Gets thrown when centroids are not loaded or trained
    """
    pass
