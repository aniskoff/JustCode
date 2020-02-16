from bs4 import BeautifulSoup
import requests
import re
import matplotlib.pyplot as plt


############################
# Some util helper classes #
############################

class Time:

    def __init__(self, s):

        splitted = re.split('[ :]', s)

        if splitted[2] == 'pm' and int(splitted[0]) < 12:
            splitted[0] = str(int(splitted[0]) + 12)

        self.hours = int(splitted[0])
        self.minutes = int(splitted[1])


class Activity:

    def __init__(self, time, type):

        self.time = time
        self.type = type


class Post:

    def __init__(self, s ,t):

        self.date = s
        self.post_time = t
        self.activity = []

    def add_activity(self, activity):

        self.activity.append(activity)


#####################################################
# Pulling Lena's activity from post comment threads #
#####################################################

class CommentActivityPuller:

    @staticmethod
    def pull_from_one_page(post, comment_soup):
        for comment in comment_soup.find_all('article', class_=re.compile('j-c j-c-(full|partial)+.*')):
            try:
                user_name = comment.find('a', class_='i-ljuser-username').text

            except AttributeError:
                continue

            if user_name == 'lena-miro.ru':

                try:
                    comment_time = Time(comment.find('span', class_='j-c-date-time').text)
                    comment_time.hours = (comment_time.hours + 3) % 24
                    post.add_activity(Activity(comment_time, 'comment'))

                except AttributeError:
                    post.add_activity(Activity(post.post_time, 'comment'))  # hidden comment case

        return post

    @staticmethod
    def pull(post, comments_soup):
        has_next_page = True

        while has_next_page:
            post = CommentActivityPuller.pull_from_one_page(post, comments_soup)

            next_page_tag = comments_soup.find('div', class_='j-comments-pages-next')

            try:
                next_page_ref = next_page_tag.a['href']
                next_page_source = requests.get(next_page_ref).text
                comments_soup = BeautifulSoup(next_page_source, 'lxml')

            except Exception:
                has_next_page = False

        return post


def main():

    page_source = requests.get("https://lena-miro.ru/").text
    page_soup = BeautifulSoup(page_source, 'lxml')

    posts = []

    article_cnt = 1
    article_limit = 90  # 1 month period (i.e. 3 articles daily)

    while article_cnt <= article_limit:

        for article in page_soup.find_all('article'):

            if article_cnt > article_limit:
                break

            comments_ref = article.find('li', class_='j-e-nav-item j-e-nav-item-comments j-p-has-icon').a['href']
            comments_source = requests.get(comments_ref).text
            comments_soup = BeautifulSoup(comments_source, 'lxml')

            day_name = article.find('span', class_='j-e-date-day').text
            post_time = article.find('span', class_='j-e-date-time').text
            post = Post(day_name, Time(post_time))
            post = CommentActivityPuller.pull(post, comments_soup)

            posts.append(post)

            article_cnt += 1
            print(article_cnt, " articles processed", '\n')

        prev_page_ref = page_soup.find('a', class_='j-page-nav-link j-page-nav-link-prev')['href']
        page_source = requests.get(prev_page_ref).text
        page_soup = BeautifulSoup(page_source, 'lxml')

    ######################################################
    # Collecting some statistics and plotting histograms #
    ######################################################

    total_daily_activity = 0

    to_plot = {i: 0 for i in range(1, 24)}
    for post in posts:
        for activity in post.activity:
            to_plot[activity.time.hours] += 1
            total_daily_activity += 1

    plt.bar(list(to_plot.keys()), to_plot.values(), color='b')
    plt.xlabel('hours')
    plt.ylabel('Lena\'s activity')
    plt.title('Lena\'s activity during one month')
    plt.show()
    print('average daily activity is: ', total_daily_activity / article_cnt)


if __name__ == '__main__':
    main()


