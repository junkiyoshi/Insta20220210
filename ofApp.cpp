#include "ofApp.h"	

//--------------------------------------------------------------
Actor::Actor(vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	this->select_index = ofRandom(location_list.size());
	while (true) {

		auto itr = find(destination_list.begin(), destination_list.end(), this->select_index);
		if (itr == destination_list.end()) {

			destination_list.push_back(this->select_index);
			break;
		}

		this->select_index = (this->select_index + 1) % location_list.size();
	}

	this->next_index = this->select_index;
}

//--------------------------------------------------------------
void Actor::update(const int& frame_span, vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	if (ofGetFrameNum() % frame_span == 0) {

		auto tmp_index = this->select_index;
		this->select_index = this->next_index;
		int retry = next_index_list[this->select_index].size();
		this->next_index = next_index_list[this->select_index][(int)ofRandom(next_index_list[this->select_index].size())];
		while (--retry > 0) {

			auto destination_itr = find(destination_list.begin(), destination_list.end(), this->next_index);
			if (destination_itr == destination_list.end()) {

				if (tmp_index != this->next_index) {

					destination_list.push_back(this->next_index);
					break;
				}
			}

			this->next_index = next_index_list[this->select_index][(this->next_index + 1) % next_index_list[this->select_index].size()];
		}
		if (retry <= 0) {

			destination_list.push_back(this->select_index);
			this->next_index = this->select_index;
		}
	}

	auto param = ofGetFrameNum() % frame_span;
	auto distance = location_list[this->next_index] - location_list[this->select_index];
	this->location = location_list[this->select_index] + distance / frame_span * param;

	this->log.push_front(this->location);
	while (this->log.size() > 60) { this->log.pop_back(); }
}

//--------------------------------------------------------------
glm::vec3 Actor::getLocation() {

	return this->location;
}

//--------------------------------------------------------------
deque<glm::vec3> Actor::getLog() {

	return this->log;
}

//--------------------------------------------------------------
void Actor::setColor(ofColor color) {

	this->color = color;
}

//--------------------------------------------------------------
ofColor Actor::getColor() {

	return this->color;
}


//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(30);
	ofSetWindowTitle("openFrameworks");

	ofBackground(0);
	ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_ADD);
	ofSetLineWidth(2);

	ofTrueTypeFont font;
	ofFbo fbo;
	ofPixels pixels;
	fbo.allocate(ofGetWidth(), ofGetHeight());
	fbo.begin();
	ofClear(0);

	font.loadFont("fonts/Kazesawa-bold.ttf", 315, true, true, true);
	string word = "18";
	font.drawString(word, ofGetWidth() * 0.5 - font.stringWidth(word) * 0.5, ofGetHeight() * 0.5 - font.stringHeight(word) * 0.05);
	word = "82";
	font.drawString(word, ofGetWidth() * 0.5 - font.stringWidth(word) * 0.5, ofGetHeight() * 0.5 + font.stringHeight(word) * 1.05);

	fbo.end();
	fbo.readToPixels(pixels);

	ofColor pix_color;
	int span = 8;
	for (float x = 0 + span * 0.5; x < ofGetWidth(); x += span) {

		for (float y = 0 + span * 0.5; y <= ofGetHeight(); y += span) {

			pix_color = pixels.getColor(x, y);
			if (pix_color != ofColor(0, 0)) {

				this->location_list.push_back(glm::vec3(x - ofGetWidth() * 0.5, y - ofGetHeight() * 0.5, 0));
			}
		}
	}

	auto param = span * sqrt(3);
	for (auto& location : this->location_list) {

		vector<int> next_index = vector<int>();
		int index = -1;
		for (auto& other : this->location_list) {

			index++;
			if (location == other) { continue; }

			float distance = glm::distance(location, other);
			if (distance <= param) {

				next_index.push_back(index);
			}
		}

		this->next_index_list.push_back(next_index);
	}

	ofColor color;
	vector<int> hex_list = { 0xef476f, 0xffd166, 0x06d6a0, 0x118ab2, 0x073b4c };
	vector<ofColor> base_color_list;
	for (auto hex : hex_list) {

		color.setHex(hex);
		base_color_list.push_back(color);
	}

	for (int i = 0; i < 360; i++) {

		this->actor_list.push_back(make_unique<Actor>(this->location_list, this->next_index_list, this->destination_list));
		this->actor_list.back()->setColor(base_color_list[(int)ofRandom(base_color_list.size())]);
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	int frame_span = 5;
	int prev_index_size = 0;

	if (ofGetFrameNum() % frame_span == 0) {

		prev_index_size = this->destination_list.size();
	}

	for (auto& actor : this->actor_list) {

		actor->update(frame_span, this->location_list, this->next_index_list, this->destination_list);
	}

	if (prev_index_size != 0) {

		this->destination_list.erase(this->destination_list.begin(), this->destination_list.begin() + prev_index_size);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofTranslate(ofGetWindowSize() * 0.5);

	for (auto& actor : this->actor_list) {

		ofSetColor(actor->getColor());
		ofNoFill();

		ofBeginShape();
		for (auto& l : actor->getLog()) {

			ofVertex(l);
		}
		ofEndShape();
	}
}


//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}