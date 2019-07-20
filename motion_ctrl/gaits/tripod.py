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

def backward(n_cycles, pause_time):
    """
    Moves the robot backwards
    n_cycles: int - The number of cycles to move foward
    pause_time: int - The number of seconds to wait between motions
    """
    for i in range(n_cycles):
        motion_b(direction="backward")
        motion_b(direction="backward")

def stop():
    pass


def motion_a(direction="forward", pause_time=0.1, *args):
    """
    Move Group A (leg_id % 2 = 1) legs
    """
    for leg in legs:
        if leg.id % 2 == 1:
            #power_stroke(leg.id)
            extend(leg.id)
        else:
            #return_stroke(leg.id)
            retract(leg.id)
        if leg.id % 2 == 1:
            pivot_backwards(leg.id)
        else:
            pivot_foward(leg.id)
    time.sleep(pause_time)


def motion_b(direction="forward", pause_time=0.1, *args):
    """
    Move Group B (leg_id % 2 = 0) legs
    """
    for leg in legs:
        if leg.id % 2 == 0:
            #power_stroke(leg.id)
            extend(leg.id)
        else:
            #return_stroke(leg.id)
            retract(leg.id)
        if leg.id % 2 == 0:
            pivot_backwards(leg.id)
        else:
            pivot_foward(leg.id)
    time.sleep(pause_time)

def power_stroke(id):
    """
    Pushes foward
    """
    pivot_backwards(id)

def return_stroke(id):
    """
    Returns the leg to the foward position
    """
    pivot_foward(id)

def stand_phase(id):
    """
    Extends the leg to the ground position
    """
    extend(id)

def swing_phase(id):
    """
    Retracts the lg to the up position
    """
    retract(id)