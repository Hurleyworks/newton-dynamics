//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 4.0.2
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class ndCollisionBvhInfo : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal ndCollisionBvhInfo(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(ndCollisionBvhInfo obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~ndCollisionBvhInfo() {
    Dispose(false);
  }

  public void Dispose() {
    Dispose(true);
    global::System.GC.SuppressFinalize(this);
  }

  protected virtual void Dispose(bool disposing) {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          newtonPINVOKE.delete_ndCollisionBvhInfo(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public SWIGTYPE_p_ndInt32 m_vertexCount {
    set {
      newtonPINVOKE.ndCollisionBvhInfo_m_vertexCount_set(swigCPtr, SWIGTYPE_p_ndInt32.getCPtr(value));
      if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      SWIGTYPE_p_ndInt32 ret = new SWIGTYPE_p_ndInt32(newtonPINVOKE.ndCollisionBvhInfo_m_vertexCount_get(swigCPtr), true);
      if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public SWIGTYPE_p_ndInt32 m_indexCount {
    set {
      newtonPINVOKE.ndCollisionBvhInfo_m_indexCount_set(swigCPtr, SWIGTYPE_p_ndInt32.getCPtr(value));
      if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      SWIGTYPE_p_ndInt32 ret = new SWIGTYPE_p_ndInt32(newtonPINVOKE.ndCollisionBvhInfo_m_indexCount_get(swigCPtr), true);
      if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public ndCollisionBvhInfo() : this(newtonPINVOKE.new_ndCollisionBvhInfo(), true) {
  }

}
