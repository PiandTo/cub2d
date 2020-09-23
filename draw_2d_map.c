/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_2d_map.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snaomi <snaomi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/20 09:12:58 by snaomi            #+#    #+#             */
/*   Updated: 2020/09/23 06:55:26 by snaomi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mlx.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "libft/libft.h"
#include "get_next_line/get_next_line.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define TILE_SIZE	32
#define WIDTH 		800
#define HEIGHT 		600
#define PI			3.14159265358979323846

typedef struct	s_img
{
	void		*ptr_img;
	int			*data;
	int			bpp;
	int			size_line;
	int			endian;
}				t_img;

typedef struct	s_mlx
{
    void		*mlx;
	void		*win;
	t_img 		img;
	char		**map;
	int			rows;
	int			cols;
}				t_mlx;

typedef struct	s_player
{
	float 		x;
	float 		y;
	float		width;
	float 		height;
	int 		turnDirection; //-1 for left, +1 for right
	int			walkDirection; // -1 for back, +1 for front
	float		rotationAngel;
	float		walkspeed;
	float 		turnSpeed;
}				t_player;


static void 	setup_player()
{
	t_player player;
	
	player.x = WIDTH / 2;
	player.y = HEIGHT / 2;
	player.width = 5;
	player.height = 5;
	player.turnDirection = 0;
	player.walkDirection = 0;
	player.rotationAngel = PI / 2;
	player.walkspeed = 100;
	player.turnSpeed = 45 * (PI / 180); 
}

static 	void	draw_player(t_mlx *game, int i, int j)
{
	int c;
	int d;
	
	c = 0;
	i *= TILE_SIZE;
	j *= TILE_SIZE;
	while(c < TILE_SIZE)
	{
		d = 0;
		while(d < TILE_SIZE)
		{
			game->img.data[(j + c) * WIDTH + i + d] = 0xFF0000;
			d++;
		}
		c++;
	}
}

static 	void	draw_rectangles(t_mlx *game, int i, int j)
{
	int c;
	int d;
	
	c = 0;
	i *= TILE_SIZE;
	j *= TILE_SIZE;
	while(c < TILE_SIZE)
	{
		d = 0;
		while(d < TILE_SIZE)
		{
				/*
				 As you can see here instead of using the mlx_put_pixel function
				 I just assign a color to each pixel one by one in the image,
				 and the image will be printed in one time at the end of the loop.
				 Now one thing to understand here is that you're working on a 1-dimensional
				 array, while your window is (obviously) 2-dimensional.
				 So, instead of having data[height][width] here you'll have the following
				 formula : [current height * max width + current width] (as you can see below)
				*/
			game->img.data[(j + c) * WIDTH + i + d] = 0x00FF00;
			d++;
		}
		c++;
	}
}

static int	find_coordinates(t_mlx *game)
{
	int i;
	int j;

	i = 0;
	while(i < (game->rows))
	{
		j = 0;
		while(j < (game->cols))
		{
			if(game->map[i][j] == '1')	
				draw_rectangles(game, j, i);
			if(game->map[i][j] == '2')	
				draw_player(game, j, i);
			j++;
		}
		i++;
	}
}

//
static int draw_line(t_mlx *game, double x1, double y1, double x2, double y2)
{
	double	deltaX;
	double	deltaY;
	double	step;
	
	deltaX = x2 - x1;
	deltaY = y2 - y1;
	step = (fabs(deltaX) > fabs(deltaY)) ? fabs(deltaX) : fabs(deltaY);
	deltaX /= step;
	deltaY /= step;
	while (fabs(x2 - x1) > 0.01 || fabs(y2 - y1) > 0.01)
	{
		game->img.data[((int)floor(y1) * WIDTH + (int)floor(x1))] = 0xb3b3b3;
		x1 += deltaX;
		y1 += deltaY;
	}
}

static int draw_lines(t_mlx *game)
{
	int i;
	int j;

	i = -1;
	j = -1;
	while(++j < game->cols)
		draw_line(game, j * TILE_SIZE, 0, j * TILE_SIZE, HEIGHT);
	draw_line(game, j * TILE_SIZE - 1, 0, j * TILE_SIZE - 1, HEIGHT);
	while(++i < game->rows)
		draw_line(game, 0, i * TILE_SIZE, WIDTH, i * TILE_SIZE);
	draw_line(game, 0, i * TILE_SIZE - 1, WIDTH, i * TILE_SIZE - 1);
}

// -------------------------Вызываем функцию, которая отрисовываем лабиринт и отображаем в окно
static int	draw_img(t_mlx *game)
{
	find_coordinates(game);
	draw_lines(game);
	mlx_put_image_to_window(game->mlx, game->win, game->img.ptr_img, 0, 0);
	return (0);
}

//-------------------------- Устанавливаем соединение с mlx, делаем ссылку на окно и image.
// ------------------------- вызываем бесконечное отображение, вызывая стороннюю функцию 
static int main_draw(t_mlx *game)
{
	// printf("%c\n", game->map[2][2]);
	game->mlx = mlx_init();
	game->win = mlx_new_window(game->mlx, WIDTH, HEIGHT, "CUB3D");
	game->img.ptr_img = mlx_new_image(game->mlx, WIDTH, HEIGHT);
	game->img.data = (int *)mlx_get_data_addr(game->img.ptr_img, &game->img.bpp, &game->img.size_line, &game->img.endian);
	mlx_loop_hook(game->mlx, &draw_img, game);
	mlx_loop(game->mlx);
	exit(0);
}

// ------------------------Перекладываем данные в 2умерный массив и оставляем только лабиринт + количество строк и столбцов лабиринта
// ------------------------Записываем карту и количество строк/столбцов в структуру game.
static void	parcer_map(t_list **head, int i, t_mlx *game)
{
	char	**buf;
	char	**buf_map;
	t_list 	*tmp;
	int		map_num_rows;
	int		map_num_cols;
	int		j;

	j = 0;
	map_num_cols = 0;
	map_num_rows = 0;
	buf = ft_calloc(sizeof(char*), (i + 1));
	buf[i] = '\0';
	tmp = *head;	
	while((tmp->next) != NULL)
	{
		buf[j] = (char*)tmp->content;
		tmp = tmp->next;
		j++;
	}
	j = 0;
	while(buf[j])
	{
		if(buf[j][0] == '1' || buf[j][0] == ' ')
			map_num_rows++;
		j++;
	}
	buf_map = ft_calloc(sizeof(char*), (map_num_rows + 1));
	j = 0;
	map_num_rows = 0;
	while(buf[j])
	{
		if(buf[j][0] == '1' || buf[j][0] == ' ')
		{

			buf_map[map_num_rows] = buf[j];
			if (map_num_cols < ft_strlen(buf_map[map_num_rows]))
				map_num_cols = ft_strlen(buf_map[map_num_rows]);
			map_num_rows++;
		}
		j++;
	}
	buf_map[map_num_rows] = '\0';
	game->map = buf_map;
	game->rows = map_num_rows;
	game->cols = map_num_cols;
	main_draw(game);
}

// ---------------------Читаем файл cub3D.cub / построчно кладем в структуру head / инициилизируем структуру game
// ---------------------Передаем структуру head и game дальше + также передаем количиство строк структуры 
int		main(int argc, char **argv)
{
	int			fd;
	char		*line;
	t_list		*head;
	t_list		*tmp;
	t_mlx		game;

	if (argc != 2)
	{
		printf("%s\n", "Error");
		return (0);
	}
	fd = open(argv[1], O_RDONLY);
	line = NULL;
	head = NULL;
	while ((get_next_line(fd, &line)) > 0)
		ft_lstadd_back(&head, ft_lstnew(line));
	ft_lstadd_back(&head, ft_lstnew(line));
	tmp = head;
	parcer_map(&head, ft_lstsize(tmp), &game);
	free(line);
	line = NULL;
	return (0);
}