namespace Graphics {

	void Init();
	void Shutdown() throw();

	void Flush();
	void Clear();
	int LoadImg(const char* img_name);
	void DrawImg(int img_id, int scx, int scy, rect_s img_rect);
	void DrawTransparentImg(int img_id, int scx, int scy, rect_s img_rect);
	void DrawTxt(int x, int y, const char* text);

};
