static const float Far = 1000;

float LogDepthBuffer(float w) {
	float Fcoef = 2.0 / log2(Far + 1.0);
	float z = log2(max(1e-6, 1.0 + w)) * Fcoef - 1.0;
	return z * w;
}