# usb-firewall

A C++ & windows.h tool that automatically detects the addition of, 
(potentially unexpected), HIDs and quarantines input until approved.

A simple, open-source, free, USB endpoint security tool for mice and keyboards.

---

### Minimum Viable Product (in-progress)
* Addition of a mouse or keyboard locks the device
* Entering the PIN unlocks the device

### Adding Value (in-order after MVP)
1. User Interface: allow application to be used by a broader audience
2. Specify Device Type: provides user information on what device was attached
    * Additions during a lock-down need to be accounted for
3. Display Input Events: allows savvy user to identify potential attack

---

### Purpose

This tool is aimed at anyone who uses a laptop running Windows and wants more
piece of mind regarding things being plugged into it.

While stationary systems and kiosks can be locked down, the typical laptop user
is less likely to fill their USB ports with cement.

---

### Functionality

The completed tool will automatically block all mouse and keyboard input upon
the insertion of a new input device.

Input from all mice and keyboards will automatically be logged, so that an 
attempted attack can be examined later.

The keying-in of a password will re-enable device input for all devices.

---

### Motivations

Software typically receives more attention when it comes to security: UIPI, UAC.
However, unless in a controlled building - input is typically neglected. Every
keyboard and mouse is assumed to be legitimate, enabling a device to grant 
itself privileges through 'legitimate' fake input. Policies have failed you if
somebody gains physical access to a system that's supposed to be secure, but, 
that's why we have insurance... We never want a system to fail, but, failure
happens. No matter the skill level of the person or robot driving a vehicle - it
is still going to be insured, have airbags, and some form of collision 
detection. 

##### Minimization of the worst-case is true resilience.
##### Assuming the worst-case will never occur is true stupidity.

---

### Limitations

This tool is currently only aimed at mice and keyboards.

Removal of all keyboards from a system could place it into a locked state, 
unless, the first keyboard after a total removal is considered to be trusted.
Which itself would be a security risk, as an attacker would remove all keyboards
and insert an an automated tool for quick attack execution. This is aimed at 
laptops as removal of the built-in keyboard is not part of a simple drive-by 
attack. Or even kiosks that have a properly secured keyboard.

---

### Trials, Tribulations, & Realizations

- Hooks are able to block input from reaching the system. USEFUL
- RawInput gives information linking input events and their origin(s). USEFUL
 
These two cannot be used together. UGH

However, this is fine. Nobody needs extensive routing of devices good and bad.
If an unexpected device is plugged into your system, you remove it.

##### KISS: Easier auditing, easier to understand, less attack surface.
###### Meet the bare requirements: if they NEEDED more, they should have asked

---

### Development Progress

- [X] Detect the addition/removal of mice/keyboards
- [X] Read and block mouse input events
- [X] Read and block keyboard input events
- [ ] Add keyboard state management to convert key strokes to text
- [ ] Convert mouse events into human-readable text
- [ ] Implement a password-entry that doesn't disclose the password in the log
- [ ] Design a log that can be added-to by both callbacks
- [ ] Combine device detection and quarantine portions
- [ ] Make the application work on a console window that pops up
- [ ] Add a user-friendly interface