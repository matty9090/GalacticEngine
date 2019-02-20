static const float Far = 100000;
static const float C = 1.0f;


float LogDepthBuffer(float w) {
    return log(C * w + 1) / log(C * Far + 1) * w;
}