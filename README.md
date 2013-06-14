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

The IGTLSessionManagerNode is wrapped by Python, so that a user can call it's API from 3D Slicer's Python Interactor.

# Example

Currently, you can push a command message from the Python Interactor. Here's a step-by-step instruction.

## Connect 3D Slicer and Robot using OpenIGTLink
We assume that the robot control software works as a TCP/IP server. Open "IGT" -> "OpenIGTLink IF", create a connector, and set IP and port for the robot control software. After making sure that the robot software is ready to connect, click "Activate" check button. If the connection is established, you will see "ON" in the Status column in the Connector list.

## Python Interaction

After opening "LightWeightRobotIGT" module, open the Python Interactor ("View" -> "Python Interactor") and type in the following lines:

    >>> from vtkSlicerLightWeightRobotIGTModuleMRMLPython import *
    >>> scene = slicer.mrmlScene
    >>> cnode = scene.GetNodeByID('vtkMRMLIGTLConnectorNode1')
    >>> sm = vtkMRMLIGTLSessionManagerNode()
    >>> scene.AddNode(sm)
    >>> sm.SetAndObserveConnectorNodeID(cnode.GetID())
    >>> snode = scene.GetNodeByID('vtkMRMLAnnotationTextNode1')

If you wat to send 'Test' command, call the following line:

    >>> snode.SetTextLabel('Test')

