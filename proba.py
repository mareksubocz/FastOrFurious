from sfml import sfml as sf

socket = sf.network.UdpSocket()
recipient = sf.network.IpAddress.get_local_address()
port = 54000
while True:
  if sf.window.Keyboard.is_key_pressed(sf.window.Keyboard.UP):
    socket.send(b"U\0", recipient, port)
  elif sf.window.Keyboard.is_key_pressed(sf.window.Keyboard.DOWN):
    socket.send(b"D\0", recipient, port)
  elif sf.window.Keyboard.is_key_pressed(sf.window.Keyboard.LEFT):
    socket.send(b"L\0", recipient, port)
  elif sf.window.Keyboard.is_key_pressed(sf.window.Keyboard.RIGHT):
    socket.send(b"R\0", recipient, port)
