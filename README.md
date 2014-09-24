LightWeightRobotIGT
===================

3D Slicer module for communication between 3D Slicer and LightWeight robot.

Session Manager Node
--------------------

# Overview

LightWeightRobotIGT manages the communication between 3D Slicer and external robot software through a IGTLSessionManagerNode node. The roles of the IGTLSessionManagerNode are as follows:

- Observes an IGTL connector node for the communication.
- Create MRML nodes that will hold incoming and outgoing message data and register them to the IGTL connector.
- Call message handler when an OpenIGTLink message has been received.


# Example



