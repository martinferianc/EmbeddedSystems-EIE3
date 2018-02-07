import micropython, array
import uctypes

class ring_buf:
  def __init__(self,size=100):
  
    self.buf = array.array('I', 0 for x in range(size))
    
    self.read_pos   = 0
    self.write_pos  = 0
    self.buf_size   = 0    

    #flags
    self.BUF_FULL   = False
    self.BUF_EMPTY  = True
    self.BUF_ERROR  = False

    self.MAX_BUF_SIZE = size

  def put(self,data):

    self.BUF_EMPTY = False    

    #check if there's an error
    if self.buf_size >= self.MAX_BUF_SIZE:
      self.BUF_FULL   = True
      return #TODO: raise error

    #write data to buffer
    self.buf[self.write_pos] = data
    self.buf_size += 1

    #increment write position
    if self.write_pos == self.MAX_BUF_SIZE-1:
      self.write_pos = 0
    else:
      self.write_pos += 1

    return

  def get(self):

    self.BUF_FULL = False

    if self.buf_size == 0:
      self.BUF_EMPTY = True
      return #TODO: raise error

    data = self.buf[self.read_pos]
    self.buf_size -= 1

    if self.read_pos == self.MAX_BUF_SIZE-1:
      self.read_pos = 0
    else:
      self.read_pos += 1

    return data

  def put_block(self,data,size):
    pass

  def get_block(self,size):
    pass

  def clear(self):
    self.write_pos = self.read_pos
    self.buf_size = 0
