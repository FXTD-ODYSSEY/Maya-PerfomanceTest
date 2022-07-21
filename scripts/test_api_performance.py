# -*- coding: utf-8 -*-
"""
create a sphere with random offset for performance test.
test in 2020 mayapy env.
"""

from __future__ import division
from __future__ import print_function
from __future__ import absolute_import

__author__ = "timmyliang"
__email__ = "820472580@qq.com"
__date__ = "2022-07-20 13:09:04"


import random
import time
import os
from maya import cmds
from maya import standalone


def log_time(func):
    def decorator(*args, **kwargs):
        curr = time.time()
        res = func(*args, **kwargs)
        print("[{0}] elapsed time: {1}".format(func.__name__, time.time() - curr))
        return res
    return decorator


@log_time
def openmaya_1(sx=50, sy=50):
    from maya import OpenMaya

    sphere = cmds.polySphere(sx=sx, sy=sy)[0]
    selection_list = OpenMaya.MSelectionList()
    OpenMaya.MGlobal.getActiveSelectionList(selection_list)
    sphere_dag_path = OpenMaya.MDagPath()
    selection_list.getDagPath(0, sphere_dag_path)
    itr = OpenMaya.MItMeshVertex(sphere_dag_path)

    while not itr.isDone():
        pt = itr.position()
        rand = (random.random() - 0.5) / 20
        itr.setPosition(pt + OpenMaya.MVector(rand, rand, rand))
        itr.next()

    return sphere


@log_time
def openmaya_2(sx=50, sy=50):
    from maya.api import OpenMaya

    sphere = cmds.polySphere(sx=sx, sy=sy)[0]
    selection_list = OpenMaya.MGlobal.getActiveSelectionList()
    sphere_dag_path = selection_list.getDagPath(0)
    itr = OpenMaya.MItMeshVertex(sphere_dag_path)

    while not itr.isDone():
        pt = itr.position()
        rand = (random.random() - 0.5) / 20
        itr.setPosition(pt + OpenMaya.MVector(rand, rand, rand))
        itr.next()
    return sphere


@log_time
def maya_cmds(sx=50, sy=50):
    from maya import cmds

    sphere = cmds.polySphere(sx=sx, sy=sy)[0]
    for vtx in cmds.ls("{0}.vtx[*]".format(sphere), fl=True):
        pt = cmds.pointPosition(vtx)
        rand = (random.random() - 0.5) / 20
        cmds.xform(vtx, t=(pt[0] + rand, pt[1] + rand, pt[2] + rand))
    return sphere


@log_time
def run_cpp(sx=50, sy=50):
    return cmds.createNoiseSphere(sx=sx, sy=sy)


def load_plugin():
    if cmds.pluginInfo("noiseSphere", q=1, l=1):
        cmds.unloadPlugin("noiseSphere")

    repo = (lambda f: f(f, os.path.dirname(__file__)))(
        lambda f, p: p
        if [d for d in os.listdir(p) if d == ".git"]
        else f(f, os.path.dirname(p))
        if os.path.dirname(p) != p
        else None
    )
    folder = "maya{0}".format(cmds.about(q=1, v=1))
    mll_path = os.path.join(repo, "release", folder, "noiseSphere.mll")
    cmds.loadPlugin(mll_path)


if __name__ == "__main__":
    standalone.initialize()
    sx = 150
    sy = 150
    load_plugin()

    maya_cmds(sx, sy)
    openmaya_1(sx, sy)
    openmaya_2(sx, sy)
    run_cpp(sx, sy)
    
    standalone.uninitialize()
