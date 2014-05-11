// UIManager.h: interface for the CUIAssist class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIASSIST_H__C68903BA_D22F_406A_8380_D302B1E3FF68__INCLUDED_)
#define AFX_UIASSIST_H__C68903BA_D22F_406A_8380_D302B1E3FF68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Map.h"

typedef enum {
	SNAP_MAN		= 1,
	SNAP_MAN_UP		= 2,
	SNAP_MAN_DOWN	= 3,
	SNAP_MAN_LEFT	= 4,
	SNAP_MAN_RIGHT	= 5,
	SNAP_BOX		= 6,
	SNAP_BOX_IN		= 7,
	SNAP_DESTINATION = 8,
	SNAP_FLOOR		= 9,
	SNAP_WALL		= 10,
	SNAP_BACKGROUND	= 11,
} ELEMENT_SNAP;

typedef struct {
	ELEMENT_SNAP snap;
	INT	bmp_id;
	HBITMAP hbmp;
} ELEMENT_UI;	

class CUIAssist  
{
public:
	CUIAssist();
	virtual ~CUIAssist();

public:
	void	set_vk(INT vk);
	void	set_map(CMap* p_boxman_map);
	HBITMAP get_bitmap_handle(INT row, INT col);
	HBITMAP get_bitmap_from_snap(ELEMENT_SNAP e_snap);
	void	set_map_in_rect(SIZE* s, POINT* origin);
	void	get_grid_rect(INT row, INT col, OUT RECT* rc_grid);
	void	set_man_position(INT row, INT col);
	void	set_man_position(INT offset);
	POINT*	get_man_position();
	void	refresh();
	SIZE	get_grids();
	void	set_selected_element(MAP_ELEMENT me);

	void	add_element_to_grid(INT row, INT col);
	void	delete_element_from_grid(INT row, INT col);
	BYTE	get_grid(INT row, INT col);
	POINT	mouse_point_to_grid(POINT* pt, POINT* origin);

	POINT	index_to_grid(INT n_index);
	INT		grid_to_index(INT row, INT col);
	void	reset_man_position();
	void	save();
	SIZE	get_map_size();
	INT		get_grid_side_len();
	POINT	get_map_origin();

	void	transfer_coordinate(POINT *pt, INT cnt, POINT* origin);
	void	transfer_coordinate(RECT *rc, INT cnt, POINT* origin, BOOL handle_rect_out = TRUE);
	void	handle_out_of_map_rect(RECT *rc);
	void	get_grid_rect_base(INT row, INT col, OUT RECT* rc_grid);
	void	enlarge_rect(RECT* rc, INT increment);

private:
	void	calculate_map_properties();

private:
	INT m_style;
	INT m_vk;
	CMap* m_p_map;
	ELEMENT_UI* m_p_element_ui;
	INT m_snap_cnt;

	SIZE m_size;
	SIZE m_map_size;
	POINT m_origin;
	POINT m_map_origin;
	INT m_grid_side;
	INT m_top_margin;
	INT m_left_margin;
	POINT m_man_position;

	MAP_ELEMENT m_selected_map_element; 
};

#endif // !defined(AFX_UIASSIST_H__C68903BA_D22F_406A_8380_D302B1E3FF68__INCLUDED_)
