
// zhangyuxinView.cpp: CzhangyuxinView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "zhangyuxin.h"
#endif

#include "zhangyuxinDoc.h"
#include "zhangyuxinView.h"
#include<math.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CzhangyuxinView

IMPLEMENT_DYNCREATE(CzhangyuxinView, CView)

BEGIN_MESSAGE_MAP(CzhangyuxinView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_COMMAND(ID_quaternion_Interpolation, &CzhangyuxinView::OnQuaternionInterpolation)
	ON_COMMAND(ID_angle_Interpolation, &CzhangyuxinView::OnAngleInterpolation)
    ON_COMMAND(ID_spherical_Quaternion, &CzhangyuxinView::OnSphericalQuaternion)
END_MESSAGE_MAP()

// CzhangyuxinView 构造/析构

CzhangyuxinView::CzhangyuxinView() noexcept
{
	// TODO: 在此处添加构造代码
	this->m_GLPixelIndex = 0;
	this->m_hGLContext = NULL;

	m_xrotation = 0.0;
	m_yrotation = 90.0;
	m_zrotation = 0.0;

	m_xrotation_begin = 0.0;
	m_yrotation_begin = 90.0;
	m_zrotation_begin = 0.0;

	m_xrotation_end = 90.0;
	m_yrotation_end = 45.0;
	m_zrotation_end = 90.0;

	t = 0;

	interpolation_mode = 1;
}

CzhangyuxinView::~CzhangyuxinView()
{
}

BOOL CzhangyuxinView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	return CView::PreCreateWindow(cs);
}

// CzhangyuxinView 绘图

void CzhangyuxinView::OnDraw(CDC* pDC)
{
	CzhangyuxinDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	wglMakeCurrent(pDC->m_hDC, m_hGLContext);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -2.0f); 

	glRotatef(m_zrotation, 0.0f, 0.0f, 1.0f);
	glRotatef(m_yrotation, 0.0f, 1.0f, 0.0f);
	glRotatef(m_xrotation, 1.0f, 0.0f, 0.0f);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	float mat_ambient[] = { 1.0f, 0.1f, 0.1f, 1.0f };
	float mat_diffuse[] = { 0.5f, 0.3f, 0.1f, 1.0f };
	float mat_specular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	float mat_emission[] = { 0.5f, 0.0f, 0.0f, 1.0f };
	float mat_shininess[] = { 30.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glColor3f(1.0f, 0.0f, 0.0f);
	auxSolidTeapot(0.5f);

	SwapBuffers(pDC->m_hDC);
	wglMakeCurrent(NULL, NULL);
}



// CzhangyuxinView 打印

BOOL CzhangyuxinView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CzhangyuxinView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CzhangyuxinView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CzhangyuxinView 诊断

#ifdef _DEBUG
void CzhangyuxinView::AssertValid() const
{
	CView::AssertValid();
}

void CzhangyuxinView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CzhangyuxinDoc* CzhangyuxinView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CzhangyuxinDoc)));
	return (CzhangyuxinDoc*)m_pDocument;
}
#endif //_DEBUG


// CzhangyuxinView 消息处理程序
BOOL CzhangyuxinView::SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR  pixelDesc = { sizeof(PIXELFORMATDESCRIPTOR), 1,
																			   PFD_DRAW_TO_WINDOW |
																			   PFD_SUPPORT_OPENGL |
																			   PFD_DOUBLEBUFFER,
																			   PFD_TYPE_RGBA,   24,  0,0,0,0,0,0,  0,  0,  0,
																			   0,0,0,0,  32,   0,  0, 0,  0,  0,0,0 };
	m_GLPixelIndex = ChoosePixelFormat(hDC, &pixelDesc);

	if (m_GLPixelIndex == 0)
	{
		return FALSE;
	}

	if (SetPixelFormat(hDC, m_GLPixelIndex, &pixelDesc) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CzhangyuxinView::CreateViewGLContext(HDC hDC)
{
	m_hGLContext = wglCreateContext(hDC);
	if (m_hGLContext == NULL) //创建失败
	{
		return FALSE;
	}

	if (wglMakeCurrent(hDC, m_hGLContext) == FALSE)
	{//选为当前RC失败
		return FALSE;
	}
	return TRUE;
}

int CzhangyuxinView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	HWND hWnd = this->GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);

	if (this->SetWindowPixelFormat(hDC) == FALSE)
	{
		return 0;
	}

	if (this->CreateViewGLContext(hDC) == FALSE)
	{
		return 0;
	}

	/////////////
	glewInit();
	glDrawBuffer(GL_BACK);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //白背景

	glClearDepth(1.0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

    /////////////////
		////////////////////////////////////////
	//如果光照定义放在这里，光源位置相对视点固定
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float light_ambient[] = { 0.1f,0.1f,0.1f,1.0f };
	float light_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
	float light_specular[] = { 1.0f,1.0f,1.0f,1.0f };
	float light_position[] = { 1.0f,1.0f,1.0f,0.0f };


	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	////////////////////////////////////////

	SetTimer(0,100, NULL);

	return 0;
}

void CzhangyuxinView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if (wglGetCurrentContext() != NULL)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (this->m_hGLContext != NULL)
	{
		wglDeleteContext(this->m_hGLContext);
		this->m_hGLContext = NULL;
	}

}

void CzhangyuxinView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	glViewport(0, 0, cx, cy);
	GLdouble aspect;
	if (cy == 0)
	{
		aspect = (GLdouble)cx;
	}
	else
	{
		aspect = (GLdouble)cx / (GLdouble)cy;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.0 * aspect, 2.0 * aspect, -2.0, 2.0, -13.0, 13.0);
	Invalidate();
}

BOOL CzhangyuxinView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//return CView::OnEraseBkgnd(pDC);
	return TRUE;
}

void CzhangyuxinView::OnTimer(UINT_PTR nIDEvent)
{
	if (interpolation_mode == 1)
	{
		// ==========欧拉角线性插值==========
		if (t < 1.0f)
		{
			t += 0.03f;
		}
		else
		{
			t = 0.0f;
		}
		m_xrotation = m_xrotation_begin + t * (m_xrotation_end - m_xrotation_begin);
		m_yrotation = m_yrotation_begin + t * (m_yrotation_end - m_yrotation_begin);
		m_zrotation = m_zrotation_begin + t * (m_zrotation_end - m_zrotation_begin);
	}
	else if (interpolation_mode == 2)
	{
		///////////////////////////////////////////////////////////////////////四元数NLERP（原来的）
		Quaternion_begin.SetEulerAngle(m_xrotation_begin, m_yrotation_begin, m_zrotation_begin);
		Quaternion_end.SetEulerAngle(m_xrotation_end, m_yrotation_end, m_zrotation_end);

		float temp = Quaternion_begin.qw * Quaternion_end.qw
			+ Quaternion_begin.qx * Quaternion_end.qx
			+ Quaternion_begin.qy * Quaternion_end.qy
			+ Quaternion_begin.qz * Quaternion_end.qz;
		if (temp < 0.0f)
		{
			Quaternion_end.qw *= -1.0f;
			Quaternion_end.qx *= -1.0f;
			Quaternion_end.qy *= -1.0f;
			Quaternion_end.qz *= -1.0f;
		}

		if (t < 1.0f)
		{
			t += 0.03f;
		}
		else
		{
			t = 0.0f;
		}

		Quaternion_middle.qw = (1.0f - t) * Quaternion_begin.qw + t * Quaternion_end.qw;
		Quaternion_middle.qx = (1.0f - t) * Quaternion_begin.qx + t * Quaternion_end.qx;
		Quaternion_middle.qy = (1.0f - t) * Quaternion_begin.qy + t * Quaternion_end.qy;
		Quaternion_middle.qz = (1.0f - t) * Quaternion_begin.qz + t * Quaternion_end.qz;

		float len = sqrt(Quaternion_middle.qw * Quaternion_middle.qw
			+ Quaternion_middle.qx * Quaternion_middle.qx
			+ Quaternion_middle.qy * Quaternion_middle.qy
			+ Quaternion_middle.qz * Quaternion_middle.qz);
		if (len > 1e-6f)
		{
			Quaternion_middle.qw /= len;
			Quaternion_middle.qx /= len;
			Quaternion_middle.qy /= len;
			Quaternion_middle.qz /= len;
		}

		Quaternion_middle.GetEulerAngle(m_xrotation, m_yrotation, m_zrotation);
		/////////////////////////////////////////////////////////////////////////////////
	}
	else if (interpolation_mode == 3)
	{
		//////////////////////// Slerp 球面四元数插值 新增分支 ////////////////////////
		Quaternion_begin.SetEulerAngle(m_xrotation_begin, m_yrotation_begin, m_zrotation_begin);
		Quaternion_end.SetEulerAngle(m_xrotation_end, m_yrotation_end, m_zrotation_end);

		float dot = Quaternion_begin.qw * Quaternion_end.qw
			+ Quaternion_begin.qx * Quaternion_end.qx
			+ Quaternion_begin.qy * Quaternion_end.qy
			+ Quaternion_begin.qz * Quaternion_end.qz;

		// 如果点积为负，取反四元数，走最短圆弧
		if (dot < 0.0f)
		{
			dot = -dot;
			Quaternion_end.qw *= -1.0f;
			Quaternion_end.qx *= -1.0f;
			Quaternion_end.qy *= -1.0f;
			Quaternion_end.qz *= -1.0f;
		}

		if (t < 1.0f)
		{
			t += 0.03f; // 和另外两个保持同样步长，动画时长一致
		}
		else
		{
			t = 0.0f;
		}

		// Slerp核心公式
		float theta = acosf(dot);
		float sinTheta = sinf(theta);
		float s1, s2;

		// 两个四元数几乎重合，退化到线性插值防止除0
		if (sinTheta < 1e-6f)
		{
			s1 = 1.0f - t;
			s2 = t;
		}
		else
		{
			s1 = sinf((1.0f - t) * theta) / sinTheta;
			s2 = sinf(t * theta) / sinTheta;
		}

		Quaternion_middle.qw = s1 * Quaternion_begin.qw + s2 * Quaternion_end.qw;
		Quaternion_middle.qx = s1 * Quaternion_begin.qx + s2 * Quaternion_end.qx;
		Quaternion_middle.qy = s1 * Quaternion_begin.qy + s2 * Quaternion_end.qy;
		Quaternion_middle.qz = s1 * Quaternion_begin.qz + s2 * Quaternion_end.qz;

		// Slerp结果理论上已经归一化，保险可以保留归一化
		float len = sqrt(Quaternion_middle.qw * Quaternion_middle.qw
			+ Quaternion_middle.qx * Quaternion_middle.qx
			+ Quaternion_middle.qy * Quaternion_middle.qy
			+ Quaternion_middle.qz * Quaternion_middle.qz);
		if (len > 1e-6f)
		{
			Quaternion_middle.qw /= len;
			Quaternion_middle.qx /= len;
			Quaternion_middle.qy /= len;
			Quaternion_middle.qz /= len;
		}

		Quaternion_middle.GetEulerAngle(m_xrotation, m_yrotation, m_zrotation);
		///////////////////////////////////////////////////////////////////////////////
	}

	Invalidate(FALSE);
	CView::OnTimer(nIDEvent);
}



// 菜单：四元数插值
void CzhangyuxinView::OnQuaternionInterpolation()
{
	t = 0.0f;
	interpolation_mode = 2;
	m_xrotation = m_xrotation_begin;
	m_yrotation = m_yrotation_begin;
	m_zrotation = m_zrotation_begin;
}

void CzhangyuxinView::OnAngleInterpolation()
{
	t = 0.0f;
	interpolation_mode = 1;
	m_xrotation = m_xrotation_begin;
	m_yrotation = m_yrotation_begin;
	m_zrotation = m_zrotation_begin;
}



void CzhangyuxinView::OnSphericalQuaternion()
{
	// TODO: 在此添加命令处理程序代码
	t = 0.0f;
	interpolation_mode = 3;  // mode=3 代表 Slerp球面四元数插值
	// 重置角度回到起点
	m_xrotation = m_xrotation_begin;
	m_yrotation = m_yrotation_begin;
	m_zrotation = m_zrotation_begin;
}
