//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 4.0.2
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class ndBodyPlayerCapsule : ndBodyKinematic {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal ndBodyPlayerCapsule(global::System.IntPtr cPtr, bool cMemoryOwn) : base(newtonPINVOKE.ndBodyPlayerCapsule_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(ndBodyPlayerCapsule obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  protected override void Dispose(bool disposing) {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          newtonPINVOKE.delete_ndBodyPlayerCapsule(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      base.Dispose(disposing);
    }
  }

  public ndBodyPlayerCapsule(SWIGTYPE_p_ndLoadSaveBase__ndLoadDescriptor desc) : this(newtonPINVOKE.new_ndBodyPlayerCapsule__SWIG_0(SWIGTYPE_p_ndLoadSaveBase__ndLoadDescriptor.getCPtr(desc)), true) {
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public ndBodyPlayerCapsule(ndMatrix localAxis, SWIGTYPE_p_ndFloat32 mass, SWIGTYPE_p_ndFloat32 radius, SWIGTYPE_p_ndFloat32 height, SWIGTYPE_p_ndFloat32 stepHeight) : this(newtonPINVOKE.new_ndBodyPlayerCapsule__SWIG_1(ndMatrix.getCPtr(localAxis), SWIGTYPE_p_ndFloat32.getCPtr(mass), SWIGTYPE_p_ndFloat32.getCPtr(radius), SWIGTYPE_p_ndFloat32.getCPtr(height), SWIGTYPE_p_ndFloat32.getCPtr(stepHeight)), true) {
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public override ndBodyKinematic GetAsBodyPlayerCapsule() {
    global::System.IntPtr cPtr = newtonPINVOKE.ndBodyPlayerCapsule_GetAsBodyPlayerCapsule(swigCPtr);
    ndBodyPlayerCapsule ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndBodyPlayerCapsule(cPtr, false);
    return ret;
  }

  public SWIGTYPE_p_ndFloat32 GetForwardSpeed() {
    SWIGTYPE_p_ndFloat32 ret = new SWIGTYPE_p_ndFloat32(newtonPINVOKE.ndBodyPlayerCapsule_GetForwardSpeed(swigCPtr), true);
    return ret;
  }

  public void SetForwardSpeed(SWIGTYPE_p_ndFloat32 speed) {
    newtonPINVOKE.ndBodyPlayerCapsule_SetForwardSpeed(swigCPtr, SWIGTYPE_p_ndFloat32.getCPtr(speed));
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public SWIGTYPE_p_ndFloat32 GetLateralSpeed() {
    SWIGTYPE_p_ndFloat32 ret = new SWIGTYPE_p_ndFloat32(newtonPINVOKE.ndBodyPlayerCapsule_GetLateralSpeed(swigCPtr), true);
    return ret;
  }

  public void SetLateralSpeed(SWIGTYPE_p_ndFloat32 speed) {
    newtonPINVOKE.ndBodyPlayerCapsule_SetLateralSpeed(swigCPtr, SWIGTYPE_p_ndFloat32.getCPtr(speed));
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public SWIGTYPE_p_ndFloat32 GetHeadingAngle() {
    SWIGTYPE_p_ndFloat32 ret = new SWIGTYPE_p_ndFloat32(newtonPINVOKE.ndBodyPlayerCapsule_GetHeadingAngle(swigCPtr), true);
    return ret;
  }

  public void SetHeadingAngle(SWIGTYPE_p_ndFloat32 angle) {
    newtonPINVOKE.ndBodyPlayerCapsule_SetHeadingAngle(swigCPtr, SWIGTYPE_p_ndFloat32.getCPtr(angle));
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public bool IsOnFloor() {
    bool ret = newtonPINVOKE.ndBodyPlayerCapsule_IsOnFloor(swigCPtr);
    return ret;
  }

  public virtual void ApplyInputs(SWIGTYPE_p_ndFloat32 timestep) {
    newtonPINVOKE.ndBodyPlayerCapsule_ApplyInputs(swigCPtr, SWIGTYPE_p_ndFloat32.getCPtr(timestep));
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public virtual SWIGTYPE_p_ndFloat32 ContactFrictionCallback(ndVector position, ndVector normal, SWIGTYPE_p_ndInt32 contactId, ndBodyKinematic otherbody) {
    SWIGTYPE_p_ndFloat32 ret = new SWIGTYPE_p_ndFloat32(newtonPINVOKE.ndBodyPlayerCapsule_ContactFrictionCallback(swigCPtr, ndVector.getCPtr(position), ndVector.getCPtr(normal), SWIGTYPE_p_ndInt32.getCPtr(contactId), ndBodyKinematic.getCPtr(otherbody)), true);
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}
