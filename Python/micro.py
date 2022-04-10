from microbit import *
import math

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

def Start():
    uart.init(baudrate=31250, bits=8, parity=None, stop=1, tx=pin0)

Start()
lastA = False
lastB = False
lastC = False
BUTTON_A_NOTE = 35
BUTTON_B_NOTE = 36
BUTTON_C_NOTE = 37
POT_NOTE = 40
last_pot = 0
pot = 0
while True:
    a = button_a.is_pressed()
    b = button_b.is_pressed()
    c = pin1.is_touched()
    pot = pin2.read_analog()

    if a:
        midiNoteOn(0, BUTTON_A_NOTE, 127)
    elif not a and lastA:
        midiNoteOff(0, BUTTON_A_NOTE, 127)
    if b:
        midiNoteOn(0, BUTTON_B_NOTE, 127)
    elif not b and lastB:
        midiNoteOff(0, BUTTON_B_NOTE, 127)
    if c and not lastC:
        midiNoteOn(0, BUTTON_C_NOTE, 127)
    elif not c and lastC:
        midiNoteOff(0, BUTTON_C_NOTE,  127)

    if last_pot != pot:
        velocity = math.floor(pot / 1024 * 127)
        midiNoteOn(0, POT_NOTE, velocity)
    last_pot = pot

    lastA = a
    lastB = b
    lastC = c
    sleep(100)

