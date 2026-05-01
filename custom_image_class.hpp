
wxPoint getCenter(wxImage& img) {
   return wxPoint(img.GetWidth() / 2, img.GetHeight() / 2);
}

class CustomImage {
public:
   wxBitmap* bitmap;
   double angle;

   CustomImage(std::string& image_path) {
      bitmap = new wxBitmap(image_path, wxBITMAP_TYPE_PNG);
      angle = 0.f;
   }

   double getRadAngle() {
      return wxDegToRad(this->angle);
   }

   ~CustomImage() {
      //delete bitmap;
   }
};
