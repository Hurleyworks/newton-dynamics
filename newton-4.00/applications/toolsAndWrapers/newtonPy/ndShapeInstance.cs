//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 4.0.2
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class ndShapeInstance : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal ndShapeInstance(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(ndShapeInstance obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~ndShapeInstance() {
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
          newtonPINVOKE.delete_ndShapeInstance(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public class ndDistanceCalculator : global::System.IDisposable {
    private global::System.Runtime.InteropServices.HandleRef swigCPtr;
    protected bool swigCMemOwn;
  
    internal ndDistanceCalculator(global::System.IntPtr cPtr, bool cMemoryOwn) {
      swigCMemOwn = cMemoryOwn;
      swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
    }
  
    internal static global::System.Runtime.InteropServices.HandleRef getCPtr(ndDistanceCalculator obj) {
      return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
    }
  
    ~ndDistanceCalculator() {
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
            newtonPINVOKE.delete_ndShapeInstance_ndDistanceCalculator(swigCPtr);
          }
          swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
        }
      }
    }
  
    public ndDistanceCalculator(SWIGTYPE_p_ndScene scene) : this(newtonPINVOKE.new_ndShapeInstance_ndDistanceCalculator__SWIG_0(SWIGTYPE_p_ndScene.getCPtr(scene)), true) {
    }
  
    public ndDistanceCalculator(SWIGTYPE_p_ndScene scene, ndShapeInstance shape0, ndMatrix matrix0, ndShapeInstance shape1, ndMatrix matrix1) : this(newtonPINVOKE.new_ndShapeInstance_ndDistanceCalculator__SWIG_1(SWIGTYPE_p_ndScene.getCPtr(scene), ndShapeInstance.getCPtr(shape0), ndMatrix.getCPtr(matrix0), ndShapeInstance.getCPtr(shape1), ndMatrix.getCPtr(matrix1)), true) {
      if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
    }
  
    public bool ClosestPoint() {
      bool ret = newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_ClosestPoint(swigCPtr);
      return ret;
    }
  
    public ndMatrix m_matrix0 {
      set {
        newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_matrix0_set(swigCPtr, ndMatrix.getCPtr(value));
      } 
      get {
        global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_matrix0_get(swigCPtr);
        ndMatrix ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndMatrix(cPtr, false);
        return ret;
      } 
    }
  
    public ndMatrix m_matrix1 {
      set {
        newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_matrix1_set(swigCPtr, ndMatrix.getCPtr(value));
      } 
      get {
        global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_matrix1_get(swigCPtr);
        ndMatrix ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndMatrix(cPtr, false);
        return ret;
      } 
    }
  
    public ndVector m_point0 {
      set {
        newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_point0_set(swigCPtr, ndVector.getCPtr(value));
      } 
      get {
        global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_point0_get(swigCPtr);
        ndVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndVector(cPtr, false);
        return ret;
      } 
    }
  
    public ndVector m_point1 {
      set {
        newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_point1_set(swigCPtr, ndVector.getCPtr(value));
      } 
      get {
        global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_point1_get(swigCPtr);
        ndVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndVector(cPtr, false);
        return ret;
      } 
    }
  
    public ndVector m_normal {
      set {
        newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_normal_set(swigCPtr, ndVector.getCPtr(value));
      } 
      get {
        global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_normal_get(swigCPtr);
        ndVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndVector(cPtr, false);
        return ret;
      } 
    }
  
    public SWIGTYPE_p_ndScene m_scene {
      set {
        newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_scene_set(swigCPtr, SWIGTYPE_p_ndScene.getCPtr(value));
      } 
      get {
        global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_scene_get(swigCPtr);
        SWIGTYPE_p_ndScene ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_ndScene(cPtr, false);
        return ret;
      } 
    }
  
    public ndShapeInstance m_shape0 {
      set {
        newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_shape0_set(swigCPtr, ndShapeInstance.getCPtr(value));
      } 
      get {
        global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_shape0_get(swigCPtr);
        ndShapeInstance ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndShapeInstance(cPtr, false);
        return ret;
      } 
    }
  
    public ndShapeInstance m_shape1 {
      set {
        newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_shape1_set(swigCPtr, ndShapeInstance.getCPtr(value));
      } 
      get {
        global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_ndDistanceCalculator_m_shape1_get(swigCPtr);
        ndShapeInstance ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndShapeInstance(cPtr, false);
        return ret;
      } 
    }
  
  }

  public ndShapeInstance(ndShape shape) : this(newtonPINVOKE.new_ndShapeInstance__SWIG_0(ndShape.getCPtr(shape)), true) {
  }

  public ndShapeInstance(ndShapeInstance instance) : this(newtonPINVOKE.new_ndShapeInstance__SWIG_1(ndShapeInstance.getCPtr(instance)), true) {
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public ndShapeInstance(ndShapeInstance instance, ndShape shape) : this(newtonPINVOKE.new_ndShapeInstance__SWIG_2(ndShapeInstance.getCPtr(instance), ndShape.getCPtr(shape)), true) {
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public ndShapeInstance(SWIGTYPE_p_nd__TiXmlNode xmlNode, SWIGTYPE_p_ndShapeLoaderCache shapesMap) : this(newtonPINVOKE.new_ndShapeInstance__SWIG_3(SWIGTYPE_p_nd__TiXmlNode.getCPtr(xmlNode), SWIGTYPE_p_ndShapeLoaderCache.getCPtr(shapesMap)), true) {
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public ndShapeInstance Assigment(ndShapeInstance src) {
    ndShapeInstance ret = new ndShapeInstance(newtonPINVOKE.ndShapeInstance_Assigment(swigCPtr, ndShapeInstance.getCPtr(src)), false);
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public ndMatrix CalculateInertia() {
    ndMatrix ret = new ndMatrix(newtonPINVOKE.ndShapeInstance_CalculateInertia(swigCPtr), true);
    return ret;
  }

  public void CalculateObb(ndVector origin, ndVector size) {
    newtonPINVOKE.ndShapeInstance_CalculateObb(swigCPtr, ndVector.getCPtr(origin), ndVector.getCPtr(size));
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public void CalculateAabb(ndMatrix matrix, ndVector minP, ndVector maxP) {
    newtonPINVOKE.ndShapeInstance_CalculateAabb(swigCPtr, ndMatrix.getCPtr(matrix), ndVector.getCPtr(minP), ndVector.getCPtr(maxP));
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public void DebugShape(ndMatrix matrix, ndShapeDebugNotify debugCallback) {
    newtonPINVOKE.ndShapeInstance_DebugShape(swigCPtr, ndMatrix.getCPtr(matrix), ndShapeDebugNotify.getCPtr(debugCallback));
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public SWIGTYPE_p_ndFloat32 RayCast(ndRayCastNotify callback, ndVector localP0, ndVector localP1, ndBody body, SWIGTYPE_p_ndContactPoint contactOut) {
    SWIGTYPE_p_ndFloat32 ret = new SWIGTYPE_p_ndFloat32(newtonPINVOKE.ndShapeInstance_RayCast(swigCPtr, ndRayCastNotify.getCPtr(callback), ndVector.getCPtr(localP0), ndVector.getCPtr(localP1), ndBody.getCPtr(body), SWIGTYPE_p_ndContactPoint.getCPtr(contactOut)), true);
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public ndShapeInfo GetShapeInfo() {
    ndShapeInfo ret = new ndShapeInfo(newtonPINVOKE.ndShapeInstance_GetShapeInfo(swigCPtr), true);
    return ret;
  }

  public void Save(SWIGTYPE_p_ndLoadSaveBase__ndSaveDescriptor desc) {
    newtonPINVOKE.ndShapeInstance_Save(swigCPtr, SWIGTYPE_p_ndLoadSaveBase__ndSaveDescriptor.getCPtr(desc));
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public SWIGTYPE_p_ndFloat32 CalculateBuoyancyCenterOfPresure(ndVector com, ndMatrix matrix, ndVector fluidPlane) {
    SWIGTYPE_p_ndFloat32 ret = new SWIGTYPE_p_ndFloat32(newtonPINVOKE.ndShapeInstance_CalculateBuoyancyCenterOfPresure(swigCPtr, ndVector.getCPtr(com), ndMatrix.getCPtr(matrix), ndVector.getCPtr(fluidPlane)), true);
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static ndVector GetBoxPadding() {
    ndVector ret = new ndVector(newtonPINVOKE.ndShapeInstance_GetBoxPadding(), true);
    return ret;
  }

  public ndShape GetShape() {
    global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_GetShape__SWIG_0(swigCPtr);
    ndShape ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndShape(cPtr, false);
    return ret;
  }

  public void SetShape(ndShape shape) {
    newtonPINVOKE.ndShapeInstance_SetShape(swigCPtr, ndShape.getCPtr(shape));
  }

  public ndVector SupportVertex(ndVector dir) {
    ndVector ret = new ndVector(newtonPINVOKE.ndShapeInstance_SupportVertex(swigCPtr, ndVector.getCPtr(dir)), true);
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public ndMatrix GetScaledTransform(ndMatrix matrix) {
    ndMatrix ret = new ndMatrix(newtonPINVOKE.ndShapeInstance_GetScaledTransform(swigCPtr, ndMatrix.getCPtr(matrix)), true);
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public ndVector SupportVertexSpecial(ndVector dir, SWIGTYPE_p_ndInt32 vertexIndex) {
    ndVector ret = new ndVector(newtonPINVOKE.ndShapeInstance_SupportVertexSpecial(swigCPtr, ndVector.getCPtr(dir), SWIGTYPE_p_ndInt32.getCPtr(vertexIndex)), true);
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public ndVector SupportVertexSpecialProjectPoint(ndVector point, ndVector dir) {
    ndVector ret = new ndVector(newtonPINVOKE.ndShapeInstance_SupportVertexSpecialProjectPoint(swigCPtr, ndVector.getCPtr(point), ndVector.getCPtr(dir)), true);
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public ndMatrix GetLocalMatrix() {
    ndMatrix ret = new ndMatrix(newtonPINVOKE.ndShapeInstance_GetLocalMatrix(swigCPtr), false);
    return ret;
  }

  public void SetLocalMatrix(ndMatrix matrix) {
    newtonPINVOKE.ndShapeInstance_SetLocalMatrix(swigCPtr, ndMatrix.getCPtr(matrix));
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public ndMatrix GetGlobalMatrix() {
    ndMatrix ret = new ndMatrix(newtonPINVOKE.ndShapeInstance_GetGlobalMatrix(swigCPtr), false);
    return ret;
  }

  public void SetGlobalMatrix(ndMatrix scale) {
    newtonPINVOKE.ndShapeInstance_SetGlobalMatrix(swigCPtr, ndMatrix.getCPtr(scale));
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public bool GetCollisionMode() {
    bool ret = newtonPINVOKE.ndShapeInstance_GetCollisionMode(swigCPtr);
    return ret;
  }

  public void SetCollisionMode(bool mode) {
    newtonPINVOKE.ndShapeInstance_SetCollisionMode(swigCPtr, mode);
  }

  public SWIGTYPE_p_ndInt32 GetConvexVertexCount() {
    SWIGTYPE_p_ndInt32 ret = new SWIGTYPE_p_ndInt32(newtonPINVOKE.ndShapeInstance_GetConvexVertexCount(swigCPtr), true);
    return ret;
  }

  public ndShapeMaterial GetMaterial() {
    ndShapeMaterial ret = new ndShapeMaterial(newtonPINVOKE.ndShapeInstance_GetMaterial(swigCPtr), true);
    return ret;
  }

  public void SetMaterial(ndShapeMaterial material) {
    newtonPINVOKE.ndShapeInstance_SetMaterial(swigCPtr, ndShapeMaterial.getCPtr(material));
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public ndVector GetScale() {
    ndVector ret = new ndVector(newtonPINVOKE.ndShapeInstance_GetScale(swigCPtr), false);
    return ret;
  }

  public ndVector GetInvScale() {
    ndVector ret = new ndVector(newtonPINVOKE.ndShapeInstance_GetInvScale(swigCPtr), false);
    return ret;
  }

  public ndMatrix GetAlignmentMatrix() {
    ndMatrix ret = new ndMatrix(newtonPINVOKE.ndShapeInstance_GetAlignmentMatrix(swigCPtr), false);
    return ret;
  }

  public void SetScale(ndVector scale) {
    newtonPINVOKE.ndShapeInstance_SetScale(swigCPtr, ndVector.getCPtr(scale));
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetGlobalScale(ndVector scale) {
    newtonPINVOKE.ndShapeInstance_SetGlobalScale__SWIG_0(swigCPtr, ndVector.getCPtr(scale));
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetGlobalScale(ndMatrix scaleMatrix) {
    newtonPINVOKE.ndShapeInstance_SetGlobalScale__SWIG_1(swigCPtr, ndMatrix.getCPtr(scaleMatrix));
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
  }

  public SWIGTYPE_p_ndInt32 CalculatePlaneIntersection(ndVector normal, ndVector point, ndVector contactsOut) {
    SWIGTYPE_p_ndInt32 ret = new SWIGTYPE_p_ndInt32(newtonPINVOKE.ndShapeInstance_CalculatePlaneIntersection(swigCPtr, ndVector.getCPtr(normal), ndVector.getCPtr(point), ndVector.getCPtr(contactsOut)), true);
    if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public SWIGTYPE_p_ndFloat32 GetVolume() {
    SWIGTYPE_p_ndFloat32 ret = new SWIGTYPE_p_ndFloat32(newtonPINVOKE.ndShapeInstance_GetVolume(swigCPtr), true);
    return ret;
  }

  public SWIGTYPE_p_ndFloat32 GetBoxMinRadius() {
    SWIGTYPE_p_ndFloat32 ret = new SWIGTYPE_p_ndFloat32(newtonPINVOKE.ndShapeInstance_GetBoxMinRadius(swigCPtr), true);
    return ret;
  }

  public SWIGTYPE_p_ndFloat32 GetBoxMaxRadius() {
    SWIGTYPE_p_ndFloat32 ret = new SWIGTYPE_p_ndFloat32(newtonPINVOKE.ndShapeInstance_GetBoxMaxRadius(swigCPtr), true);
    return ret;
  }

  public ndShapeInstance.ndScaleType GetScaleType() {
    ndShapeInstance.ndScaleType ret = (ndShapeInstance.ndScaleType)newtonPINVOKE.ndShapeInstance_GetScaleType(swigCPtr);
    return ret;
  }

  public SWIGTYPE_p_ndFloat32 GetUmbraClipSize() {
    SWIGTYPE_p_ndFloat32 ret = new SWIGTYPE_p_ndFloat32(newtonPINVOKE.ndShapeInstance_GetUmbraClipSize(swigCPtr), true);
    return ret;
  }

  public SWIGTYPE_p_ndUnsigned64 GetUserDataID() {
    SWIGTYPE_p_ndUnsigned64 ret = new SWIGTYPE_p_ndUnsigned64(newtonPINVOKE.ndShapeInstance_GetUserDataID(swigCPtr), true);
    return ret;
  }

  public ndMatrix m_globalMatrix {
    set {
      newtonPINVOKE.ndShapeInstance_m_globalMatrix_set(swigCPtr, ndMatrix.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_m_globalMatrix_get(swigCPtr);
      ndMatrix ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndMatrix(cPtr, false);
      return ret;
    } 
  }

  public ndMatrix m_localMatrix {
    set {
      newtonPINVOKE.ndShapeInstance_m_localMatrix_set(swigCPtr, ndMatrix.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_m_localMatrix_get(swigCPtr);
      ndMatrix ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndMatrix(cPtr, false);
      return ret;
    } 
  }

  public ndMatrix m_alignmentMatrix {
    set {
      newtonPINVOKE.ndShapeInstance_m_alignmentMatrix_set(swigCPtr, ndMatrix.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_m_alignmentMatrix_get(swigCPtr);
      ndMatrix ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndMatrix(cPtr, false);
      return ret;
    } 
  }

  public ndVector m_scale {
    set {
      newtonPINVOKE.ndShapeInstance_m_scale_set(swigCPtr, ndVector.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_m_scale_get(swigCPtr);
      ndVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndVector(cPtr, false);
      return ret;
    } 
  }

  public ndVector m_invScale {
    set {
      newtonPINVOKE.ndShapeInstance_m_invScale_set(swigCPtr, ndVector.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_m_invScale_get(swigCPtr);
      ndVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndVector(cPtr, false);
      return ret;
    } 
  }

  public ndVector m_maxScale {
    set {
      newtonPINVOKE.ndShapeInstance_m_maxScale_set(swigCPtr, ndVector.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_m_maxScale_get(swigCPtr);
      ndVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndVector(cPtr, false);
      return ret;
    } 
  }

  public ndShapeMaterial m_shapeMaterial {
    set {
      newtonPINVOKE.ndShapeInstance_m_shapeMaterial_set(swigCPtr, ndShapeMaterial.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_m_shapeMaterial_get(swigCPtr);
      ndShapeMaterial ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndShapeMaterial(cPtr, false);
      return ret;
    } 
  }

  public ndShape m_shape {
    set {
      newtonPINVOKE.ndShapeInstance_m_shape_set(swigCPtr, ndShape.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_m_shape_get(swigCPtr);
      ndShape ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndShape(cPtr, false);
      return ret;
    } 
  }

  public ndBody m_ownerBody {
    set {
      newtonPINVOKE.ndShapeInstance_m_ownerBody_set(swigCPtr, ndBody.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_m_ownerBody_get(swigCPtr);
      ndBody ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndBody(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_void m_subCollisionHandle {
    set {
      newtonPINVOKE.ndShapeInstance_m_subCollisionHandle_set(swigCPtr, SWIGTYPE_p_void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_m_subCollisionHandle_get(swigCPtr);
      SWIGTYPE_p_void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_void(cPtr, false);
      return ret;
    } 
  }

  public ndShapeInstance m_parent {
    set {
      newtonPINVOKE.ndShapeInstance_m_parent_set(swigCPtr, ndShapeInstance.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = newtonPINVOKE.ndShapeInstance_m_parent_get(swigCPtr);
      ndShapeInstance ret = (cPtr == global::System.IntPtr.Zero) ? null : new ndShapeInstance(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_ndFloat32 m_skinMargin {
    set {
      newtonPINVOKE.ndShapeInstance_m_skinMargin_set(swigCPtr, SWIGTYPE_p_ndFloat32.getCPtr(value));
      if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      SWIGTYPE_p_ndFloat32 ret = new SWIGTYPE_p_ndFloat32(newtonPINVOKE.ndShapeInstance_m_skinMargin_get(swigCPtr), true);
      if (newtonPINVOKE.SWIGPendingException.Pending) throw newtonPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public ndShapeInstance.ndScaleType m_scaleType {
    set {
      newtonPINVOKE.ndShapeInstance_m_scaleType_set(swigCPtr, (int)value);
    } 
    get {
      ndShapeInstance.ndScaleType ret = (ndShapeInstance.ndScaleType)newtonPINVOKE.ndShapeInstance_m_scaleType_get(swigCPtr);
      return ret;
    } 
  }

  public bool m_collisionMode {
    set {
      newtonPINVOKE.ndShapeInstance_m_collisionMode_set(swigCPtr, value);
    } 
    get {
      bool ret = newtonPINVOKE.ndShapeInstance_m_collisionMode_get(swigCPtr);
      return ret;
    } 
  }

  public enum ndScaleType {
    m_unit,
    m_uniform,
    m_nonUniform,
    m_global
  }

}
