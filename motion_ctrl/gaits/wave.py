import time

def forward(n_cycles, pause_time):
    """
    Moves the robot forward
    n_cycles: int - The number of cycles to move foward
    pause_time: int - The number of seconds to wait between motions
    """
    for i in range(n_cycles):
        motion_a()
        motion_b()
        motion_c()
        motion_d()
        motion_e()
        motion_f()

def backward(n_cycles, pause_time):
    """
    Moves the robot backwards
    n_cycles: int - The number of cycles to move foward
    pause_time: int - The number of seconds to wait between motions
    """
    for i in range(n_cycles):
        motion_a(direction="backward")
        motion_b(direction="backward")
        motion_c(direction="backward")
        motion_d(direction="backward")
        motion_e(direction="backward")
        motion_f(direction="backward")

def stop():
    pass


def motion_a(direction="forward", *args):
    """
    Move leg_id = 1
    """
    pass


def motion_b(direction="forward", *args):
    """
    Move leg_id = 2
    """
    pass

def motion_c(direction="forward", *args):
    """
    Move leg_id = 3
    """
    pass

def motion_d(direction="forward", *args):
    """
    Move leg_id = 4
    """
    pass

def motion_e(direction="forward", *args):
    """
    Move leg_id = 5
    """
    pass

def motion_f(direction="forward", *args):
    """
    Move leg_id = 6
    """
    pass