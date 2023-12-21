import sys

import argparse
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
import torchvision

class OthelloNNetB1(nn.Module):
    def __init__(self, args):
        # game params
        self.board_x = self.board_y = 8
        self.action_size = 64
        self.args = args

        super(OthelloNNetB1, self).__init__()
        self.fc1 = nn.Linear(self.board_x * self.board_y, 4096)
        self.fc_bn1 = nn.BatchNorm1d(4096)

        self.fc2 = nn.Linear(4096, 2048)
        self.fc_bn2 = nn.BatchNorm1d(2048)

        self.fc3 = nn.Linear(2048, 1024)
        self.fc_bn3 = nn.BatchNorm1d(1024)

        self.fc4 = nn.Linear(1024, 512)
        self.fc_bn4 = nn.BatchNorm1d(512)

        self.fc5 = nn.Linear(512, self.action_size)

        self.fc6 = nn.Linear(512, 256)
        self.fc_bn6 = nn.BatchNorm1d(256)

        self.fc7 = nn.Linear(256, 1)

    def forward(self, s):
        #                                                           s: batch_size x board_x x board_y
        s = s.view(-1, self.board_x * self.board_y)                  # batch_size x (board_x * board_y)

        s = F.dropout(F.relu(self.fc_bn1(self.fc1(s))), p=self.args.dropout, training=self.training)  # batch_size x 4096
        s = F.dropout(F.relu(self.fc_bn2(self.fc2(s))), p=self.args.dropout, training=self.training)  # batch_size x 2048
        s = F.dropout(F.relu(self.fc_bn3(self.fc3(s))), p=self.args.dropout, training=self.training)  # batch_size x 1024
        s = F.dropout(F.relu(self.fc_bn4(self.fc4(s))), p=self.args.dropout, training=self.training)  # batch_size x 512

        pi = self.fc5(s)                                                                         # batch_size x action_size

        s = F.dropout(F.relu(self.fc_bn6(self.fc6(s))), p=self.args.dropout, training=self.training)  # batch_size x 256
        v = self.fc7(s)                                                                          # batch_size x 1

        return F.log_softmax(pi, dim=1), torch.tanh(v)


class dotdict(dict):
    def __getattr__(self, name):
        return self[name]


