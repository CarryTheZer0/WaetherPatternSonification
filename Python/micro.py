from microbit import *
import math

# send midi note on messgage
def midiNoteOn(chan, n, vel):
    MIDI_NOTE_ON = 0x90
    if chan > 15:
        return
    if n > 127:
        return
    if vel > 127:
        return
    msg = bytes([MIDI_NOTE_ON | chan, n, vel])
    uart.write(msg)

# send midi note off messgage
def midiNoteOff(chan, n, vel):
    MIDI_NOTE_OFF = 0x80
    if chan > 15:
        return
    if n > 127:
        return
    if vel > 127:
        return
    msg = bytes([MIDI_NOTE_OFF | chan, n, vel])
    uart.write(msg)

# initialise data transfer
def Start():
    uart.init(baudrate=31250, bits=8, parity=None, stop=1, tx=pin0)

Start()

# define MIDI notes used (assigned arbitrarily)
BUTTON_B_NOTE = 36
PIN_1_NOTE = 37
POT_NOTE = 40

# initialise potentiometer value to 0
pot = 0

# initialise previous values to 0/false
lastB = False
lastPin = False
last_pot = 0

while True:
    b = button_b.is_pressed()
    pin = pin1.is_touched()
    pot = pin2.read_analog()

    # detect button inputs
    if b and not lastB:
        midiNoteOn(0, BUTTON_B_NOTE, 127)
    elif not b and lastB:
        midiNoteOff(0, BUTTON_B_NOTE, 127)
    if pin and not lastPin:
        midiNoteOn(0, PIN_1_NOTE, 127)
    elif not pin and lastPin:
        midiNoteOff(0, PIN_1_NOTE,  127)

    # detect potentiometer input
    if last_pot != pot:
        # map input to MIDI notes
        velocity = 127 - math.floor(pot / 1024 * 127)
        midiNoteOn(0, POT_NOTE, velocity)

    #update previous values
    lastB = b
    lastPin = pin
    last_pot = pot
    sleep(100)

