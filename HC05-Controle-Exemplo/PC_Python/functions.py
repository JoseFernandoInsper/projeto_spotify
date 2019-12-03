import win32gui
import win32api
import os
import pywinauto as appp
import time
from ctypes import cast, POINTER
from comtypes import CLSCTX_ALL
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume

spoti = appp.Application()

devices = AudioUtilities.GetSpeakers()
interface = devices.Activate(
    IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
volume = cast(interface, POINTER(IAudioEndpointVolume))
volume.GetMasterVolumeLevel()
volume.GetVolumeRange()

###Virtual-KeyCodes###
Media_Next = 0xB0
Media_Previous = 0xB1
Media_Pause = 0xB3 ##Play/Pause
Media_Mute = 0xAD
Media_VolUp = 0xAF
Media_VolDw = 0xAE

def hwcode(Media):
	hwcode = win32api.MapVirtualKey(Media, 0)
	return hwcode

def next():
	win32api.keybd_event(Media_Next, hwcode(Media_Next))
	
def previous():
	win32api.keybd_event(Media_Previous, hwcode(Media_Previous))
	
def pause():
	win32api.keybd_event(Media_Pause, hwcode(Media_Pause))
	
def play():
	win32api.keybd_event(Media_Pause, hwcode(Media_Pause))
	
def mute():
	win32api.keybd_event(Media_Mute, hwcode(Media_Mute))

def volUp():
	win32api.keybd_event(Media_VolUp, hwcode(Media_VolUp))
	
def volDw():
	win32api.keybd_event(Media_VolDw, hwcode(Media_VolDw))

def SpotifyStart():
	spoti.start("Spotify.exe")

def volu(porcentagem):
  if(porcentagem == 10):
    volume.SetMasterVolumeLevel(-33.9, None)
  elif(porcentagem == 20):
    volume.SetMasterVolumeLevel(-23.9, None)
  elif(porcentagem == 30):
    volume.SetMasterVolumeLevel(-17.9, None)
  elif(porcentagem == 40):
    volume.SetMasterVolumeLevel(-13.8, None)
  elif(porcentagem == 50):
    volume.SetMasterVolumeLevel(-10.4, None)
  elif(porcentagem == 60):
    volume.SetMasterVolumeLevel(-7.8, None)
  elif(porcentagem == 70):
    volume.SetMasterVolumeLevel(-5.4, None)
  elif(porcentagem == 80):
    volume.SetMasterVolumeLevel(-3.4, None)
  elif(porcentagem == 90):
    volume.SetMasterVolumeLevel(-1.6, None)
  elif(porcentagem == 100):
    volume.SetMasterVolumeLevel(0, None)