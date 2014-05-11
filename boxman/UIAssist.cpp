// UIManager.cpp: implementation of the CUIAssist class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIAssist.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CUIAssist::CUIAssist()
{
	ELEMENT_UI e_ui[] = {
		{SNAP_MAN, 			IDB_MAN,          NULL},
		{SNAP_MAN_UP, 		IDB_MAN_UP,       NULL},
		{SNAP_MAN_DOWN, 	IDB_MAN_DOWN,     NULL},
		{SNAP_MAN_LEFT, 	IDB_MAN_LEFT,     NULL},
		{SNAP_MAN_RIGHT, 	IDB_MAN_RIGHT,    NULL},
		{SNAP_BOX, 			IDB_BOX,          NULL},
		{SNAP_BOX_IN, 		IDB_BOX_IN,       NULL},
		{SNAP_DESTINATION,	IDB_DESTINATION,  NULL},
		{SNAP_FLOOR, 		IDB_FLOOR,        NULL},
		{SNAP_WALL, 		IDB_WALL,		  NULL},
		{SNAP_BACKGROUND,	IDB_BACKGROUND,   NULL}
	};

	m_snap_cnt = sizeof(e_ui) / sizeof(ELEMENT_UI);
	m_p_element_ui = new ELEMENT_UI[m_snap_cnt];
	memcpy(m_p_element_ui, e_ui, sizeof(ELEMENT_UI) * m_snap_cnt);
	
	for (INT i = 0; i < m_snap_cnt; i++)
	{
		m_p_element_ui[i].hbmp = (HBITMAP) ::LoadImage(_Module.GetResourceInstance(), 
                                MAKEINTRESOURCE(m_p_element_ui[i].bmp_id),
                                IMAGE_BITMAP,
                                0,
                                0,
                                0);
	}

	m_left_margin = 5;
	m_top_margin = 5;
	
	reset_man_position();
}

CUIAssist::~CUIAssist()
{
	for (INT i = 0; i < m_snap_cnt; i++)
	{
		if (NULL != m_p_element_ui[i].hbmp)
			::DeleteObject(m_p_element_ui[i].hbmp);
	}

	delete[] m_p_element_ui;
}

HBITMAP CUIAssist::get_bitmap_handle(INT row, INT col) // game mode
{
	BYTE val = m_p_map->get_grid(row, col);
	BYTE hi4bit = val / 16;
	BYTE lo4bit = val % 16;

	ELEMENT_SNAP e_snap = SNAP_BACKGROUND;
	if(hi4bit == (MAP_ELEMENT) MAN)
	{
		if (m_man_position.x == -1)
			set_man_position(row, col);  // when first draw whole map

		switch(m_vk) 
		{
		case VK_UP:
			e_snap = SNAP_MAN_UP;
			break;
		case VK_DOWN:
			e_snap = SNAP_MAN_DOWN;
			break;
		case VK_LEFT:
			e_snap = SNAP_MAN_LEFT;
			break;
		case VK_RIGHT:
			e_snap = SNAP_MAN_RIGHT;
			break;
		default:
			e_snap = SNAP_MAN;
		}
	}
	else if(hi4bit == (MAP_ELEMENT) BOX)
	{
		if (lo4bit == (MAP_ELEMENT) DESTINATION)
			e_snap = SNAP_BOX_IN;
		else
			e_snap = SNAP_BOX;
	}
	else if(hi4bit == 0)
	{
		if (lo4bit == (MAP_ELEMENT) DESTINATION)
			e_snap = SNAP_DESTINATION;
		else if (lo4bit == (MAP_ELEMENT) FLOOR)
			e_snap = SNAP_FLOOR;
		else if(lo4bit == (MAP_ELEMENT) WALL)
			e_snap = SNAP_WALL;
	}
	else
		e_snap = SNAP_BACKGROUND;

	HBITMAP hbmp = get_bitmap_from_snap(e_snap);

	return hbmp;
}

void CUIAssist::set_map(CMap* p_boxman_map)
{
	m_p_map = p_boxman_map;
}

HBITMAP CUIAssist::get_bitmap_from_snap(ELEMENT_SNAP e_snap)
{
	for (INT i = 0; i < m_snap_cnt; i++)
	{
		if (m_p_element_ui[i].snap == e_snap)
			return m_p_element_ui[i].hbmp;
	}

	return m_p_element_ui[m_snap_cnt - 1].hbmp;
}

void CUIAssist::set_map_in_rect(SIZE* s, POINT* origin)
{
	m_size.cx = s->cx;
	m_size.cy = s->cy;
	m_origin.x = origin->x;
	m_origin.y = origin->y;
	calculate_map_properties();
}

void CUIAssist::refresh()
{
	calculate_map_properties();
}

void CUIAssist::calculate_map_properties()
{
	PASS_HEADER* ph		= m_p_map->m_p_ph;
	INT n_grid_width	= m_size.cx / ph->cols;
	INT n_grid_height	= m_size.cy / ph->rows;
	m_grid_side			= min(n_grid_width, n_grid_height);

	if (m_grid_side > 32)
		m_grid_side = 32;
	else if (m_grid_side < 16)
		m_grid_side = 16;

	m_map_size.cx = m_grid_side * ph->cols;
	m_map_size.cy = m_grid_side * ph->rows;
	
	if (m_size.cx > m_map_size.cx)
		m_left_margin = (m_size.cx - m_map_size.cx) / 2;
	
	if (m_size.cy > m_map_size.cy)
		m_top_margin = (m_size.cy - m_map_size.cy) / 2;
	
	m_map_origin.x = m_left_margin;
	m_map_origin.y = m_top_margin;
}

void CUIAssist::get_grid_rect(INT row, INT col, OUT RECT* rc_grid)
{
	get_grid_rect_base(row, col, rc_grid);
	transfer_coordinate(rc_grid, 1, &m_map_origin);
}

void CUIAssist::get_grid_rect_base(INT row, INT col, OUT RECT* rc_grid)
{
	rc_grid->left	= col * m_grid_side;
	rc_grid->top	= row * m_grid_side;
	rc_grid->right	= rc_grid->left + m_grid_side;	
	rc_grid->bottom = rc_grid->top + m_grid_side;
}

void CUIAssist::set_man_position(INT row, INT col)
{
	m_man_position.x = col;
	m_man_position.y = row;
}

void CUIAssist::reset_man_position()
{
	m_man_position.x = -1;
	m_man_position.y = -1;
}

void CUIAssist::set_man_position(INT offset)
{
	POINT pt = index_to_grid(offset);
	set_man_position(pt.y, pt.x);
}

POINT* CUIAssist::get_man_position()
{
	return &m_man_position;
}

void CUIAssist::set_vk(INT vk)
{
	m_vk = vk;
}

SIZE CUIAssist::get_grids()
{
	SIZE s = {m_p_map->m_p_ph->cols, m_p_map->m_p_ph->rows};
	return s;
}

void CUIAssist::set_selected_element(MAP_ELEMENT me)
{
	m_selected_map_element = me;
}

void CUIAssist::delete_element_from_grid(INT row, INT col)
{
	BYTE val = 0;
	m_p_map->set_grid(row, col, val);
}

void CUIAssist::add_element_to_grid(INT row, INT col)
{
	BYTE me_val = 0;
	switch (m_selected_map_element) {
	case MAN:
		me_val = MAN * 16;
		break;
	case FLOOR:
		me_val = FLOOR;
		break;
	case BOX:
		me_val = BOX * 16;
		break;
	case DESTINATION:
		me_val = DESTINATION;
		break;
	case WALL:
		me_val = WALL;
		break;
	default:
		break;
	}

	BYTE  val = m_p_map->get_grid(row, col);
	val = val % 16;
	if (val == 0)
	{
		if (me_val > 16)
			val = me_val + FLOOR;
		else
			val = me_val;
	}
	else
	{
		if (me_val > 16)
		{
			if (val == WALL)
				val = me_val + FLOOR;
			else
				val = me_val + val;
		}
		else
		{
			val = me_val;
		}
	}

	m_p_map->set_grid(row, col, val);
}

BYTE CUIAssist::get_grid(INT row, INT col)
{
	return m_p_map->get_grid(row, col);
}

POINT CUIAssist::mouse_point_to_grid(POINT* pt, POINT* origin)
{
	pt->x = pt->x - m_map_origin.x - origin->x;
	pt->y = pt->y - m_map_origin.y - origin->y;
	
	POINT grid = {-1, -1};
	if (pt->x < 0
		|| pt->x >= m_map_size.cx
		|| pt->y < 0
		|| pt->y >= m_map_size.cy)
	{
		return grid;		
	}

	grid.y = pt->y / m_grid_side;
	grid.x = pt->x / m_grid_side;
	
	return grid;
}

void CUIAssist::save()
{
	m_p_map->get_valid_data();
}

POINT CUIAssist::index_to_grid(INT n_index)
{
	POINT grid;
	grid.y = n_index / m_p_map->m_p_ph->cols;
	grid.x = n_index % m_p_map->m_p_ph->cols;

	return grid;
}

INT CUIAssist::grid_to_index(INT row, INT col)
{
	INT offset = row * m_p_map->m_p_ph->cols + col;

	return offset;
}

SIZE CUIAssist::get_map_size()
{
	return m_map_size;
}

INT	CUIAssist::get_grid_side_len()
{
	return m_grid_side;
}

POINT CUIAssist::get_map_origin()
{
	return m_map_origin;
}

void CUIAssist::transfer_coordinate(POINT *pt, INT cnt, POINT* origin)
{
	for (INT i = 0; i < cnt; i ++)
	{
		(pt + i)->x += origin->x;
		(pt + i)->y += origin->y;
	}
}

void CUIAssist::transfer_coordinate(RECT *rc, INT cnt, POINT* origin, BOOL handle_rect_out)
{
	for (INT i = 0; i < cnt; i ++)
	{
		(rc + i)->left		+= origin->x;
		(rc + i)->top		+= origin->y;
		(rc + i)->right		+= origin->x;
		(rc + i)->bottom	+= origin->y;
		
		if (handle_rect_out)
			handle_out_of_map_rect(rc + i);
	}
}

void CUIAssist::handle_out_of_map_rect(RECT *rc)
{
	POINT pt = {m_size.cx, m_size.cy};
	transfer_coordinate(&pt, 1, &m_origin);
	if (rc->left > pt.x || rc->top > pt.y)
	{
		ZeroMemory(rc, sizeof(RECT));
		return;
	}
	
	if (rc->right > pt.x)
		rc->right = pt.x;

	if (rc->bottom > pt.y)
		rc->bottom = pt.y;
}

void CUIAssist::enlarge_rect(RECT* rc, INT increment)
{
	rc->left	-= increment;
	rc->top		-= increment;
	rc->right	+= increment;
	rc->bottom	+= increment;
}